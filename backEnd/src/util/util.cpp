#include "util.h"
#include "rc.h"
#include "setting.h"
#include "staticFile.h"
#include "zip.h"
#include <functional>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

#define HTTP_HEADER_BROWSER_STR \
    R"(Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36)"

void configCurl(CURL* curl) {
    // 存放ca证书数据 数据从https://curl.se/ca/cacert.pem下载 压缩后内嵌在程序中
    vector<char> caData;
    string       mineType;

    if (staticFileGetFile(CA_FILENAME, caData, mineType) != 0) {
        logE("not find ca file: %s", CA_FILENAME);
    }

    struct curl_blob ca_blob{};
    ca_blob.data  = (void*)caData.data();
    ca_blob.len   = caData.size();
    ca_blob.flags = CURL_BLOB_COPY; // 复制数据，libcurl 管理内存
    curl_easy_setopt(curl, CURLOPT_CAINFO_BLOB, &ca_blob);

    auto config = Setting::getConfig();
    //    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_PROXY, config.proxyUrl.c_str());

    // https请求认证证书
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // 模拟浏览器发起请求 携带特征头
    curl_easy_setopt(curl, CURLOPT_USERAGENT, HTTP_HEADER_BROWSER_STR);

    // 启动自动重定向
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);

    // 使用默认的响应处理回调
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlRespStr);
}

size_t curlRespStr(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

size_t curlRespFile(void* contents, size_t size, size_t nmemb, FILE** userp) {
    fwrite(contents, size, nmemb, *userp);
    return size * nmemb;
}

void writeData2File(const std::string& entryName, unsigned char* data, size_t dataLen) {
    fs::path filePath = entryName;

    logD("write data to file: %s", filePath.string().c_str());

    // 创建所需的目录
    fs::path parentPath = filePath.parent_path();
    if (!parentPath.empty()) {
        std::error_code ec;
        if (!fs::create_directories(parentPath, ec) && ec) {
            logE("Failed to create directories: %s, error: %s",
                 parentPath.string().c_str(),
                 ec.message().c_str());
            return;
        }
    }

    // 打开文件准备写入
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        logE("Failed to open file for writing: %s", filePath.string().c_str());
        return;
    }

    // 写入数据
    if (!file.write(reinterpret_cast<const char*>(data), (long long)dataLen)) {
        logE("Failed to write data to file: %s", filePath.string().c_str());
        return;
    }

    file.close();
}

int decompressZipData(
    const unsigned char* compressed_data,
    size_t               compressed_length,
    const std::function<void(std::string entryName, unsigned char* data, size_t dataLen)>&
        callback) {
    zip_error_t     error;
    zip_source_t*   src         = nullptr;
    zip_t*          za          = nullptr;
    const char*     entry_name  = nullptr;
    struct zip_stat stat        = {0};
    unsigned char*  output_data = nullptr;
    zip_file_t*     file        = nullptr;
    int             ret         = -1;
    int             num_entries = 0;

    zip_error_init(&error);

    /* create source from buffer */
    if ((src = zip_source_buffer_create((void*)compressed_data, compressed_length, 1, &error)) ==
        nullptr) {
        logE("can't create source: %s", zip_error_strerror(&error));
        goto exit;
    }

    // 从 zip_source 打开 ZIP 文件
    za = zip_open_from_source(src, 0, &error);
    if (za == nullptr) {
        logE("can't open zip from source: %s", zip_error_strerror(&error));
        goto exit;
    }

    // 获取 ZIP 文件中的条目数
    num_entries = (int)zip_get_num_entries(za, 0);
    if (num_entries < 0) {
        logE("zip_get_num_entries failed!");
        goto exit;
    }

    logD("zip file entries: %d", num_entries);

    // 遍历条目并解压
    for (int i = 0; i < num_entries; i++) {
        // 获取条目的名称
        entry_name = zip_get_name(za, i, 0);
        if (entry_name == nullptr) {
            logE("get entry name failed, err[%s]", zip_strerror(za));
            continue;
        }
        logD("find a file: %s", entry_name);

        // 获取当前条目的文件信息
        if (zip_stat_index(za, i, 0, &stat) < 0) {
            logE("get entry stat failed, err[%s]", zip_strerror(za));
            continue;
        }

        // 分配内存来存储解压后的数据
        output_data = (unsigned char*)malloc(stat.size);
        if (output_data == nullptr) {
            logE("malloc failed, size[%llu]", stat.size);
            continue;
        }

        // 解压数据
        file = zip_fopen_index(za, i, 0);
        if (file == nullptr) {
            logE("zip_fopen_index failed, err[%s]", zip_strerror(za));
            free(output_data);
            continue;
        }

        // 读取解压后的数据
        if (zip_fread(file, output_data, stat.size) < 0) {
            logE("zip_fread failed, err[%s]", zip_strerror(za));
            free(output_data);
            zip_fclose(file);
            continue;
        }

        // 处理解压后的数据
        printf("uncompress %s ok，size: %llu\n", entry_name, stat.size);

        callback(entry_name, output_data, stat.size);

        // 关闭文件
        zip_fclose(file);

        // 释放内存
        free(output_data);
    }

    // 成功
    ret = 0;

exit:
    zip_error_fini(&error);

    return ret;
}

int readFileToVector(const fs::path& filePath, std::vector<char>& buf) {
    // 打开文件
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        logE("Failed to open file: %s", filePath.string().c_str());
        return -1;
    }

    // 获取文件大小
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    buf.resize(size);

    // 读取文件内容
    if (!file.read(buf.data(), size)) {
        logE("read file error: %s", filePath.string().c_str());
        return -1;
    }

    return 0;
}

int decompressZipData(
    const std::wstring& filepath,
    const std::function<void(std::string entryName, unsigned char* data, size_t dataLen)>&
        callback) {
    vector<char> buf;

    if (readFileToVector(filepath, buf) != 0) {
        logE("read file error: %ls", filepath.c_str());
        return -1;
    }

    return decompressZipData(
        reinterpret_cast<const unsigned char*>(buf.data()), buf.size(), callback);
}

void restartProgram() {
    logW("restart server");

#ifdef WIN32
    auto filePath = getExecutablePath();

    // 使用 ShellExecuteW 执行
    HINSTANCE result = ShellExecuteW(nullptr,          // 父窗口句柄
                                     L"open",          // 操作（open 表示运行）
                                     filePath.c_str(), // 可执行文件路径
                                     nullptr,          // 参数
                                     nullptr,          // 工作目录（默认）
                                     SW_SHOW           // 显示方式
    );

    if (reinterpret_cast<intptr_t>(result) <= 32) {
        logE("ShellExecuteW failed, error: %lu", GetLastError());
    }

    // 终止当前进程
    ExitProcess(0);
#else

#endif
}

std::wstring getExecutablePath() {
    wchar_t path[MAX_PATH] = {0};
    // 获取当前可执行文件的完整路径
    DWORD size = GetModuleFileNameW(nullptr, path, MAX_PATH);
    if (size == 0) {
        logE("get executable path failed, error: %lu", GetLastError());
        return L"";
    }
    return path;
}

std::wstring string_to_wstring(const std::string& str) {
    if (str.empty())
        return {};

    // 计算所需宽字符长度
    int          size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size_needed, 0);

    // 转换
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    return wstr;
}

std::string wstring_to_string(const std::wstring& wstr) {
    if (wstr.empty())
        return {};

    // 计算所需多字节长度
    int size_needed =
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);

    // 转换
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
    return str;
}