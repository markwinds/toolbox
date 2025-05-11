#include "staticFile.h"
#include "log.h"
#include "rc.h"
#include "util.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <zip.h>

#define FILE_PREFIX "dist/"

using namespace std;

struct FileInfo {
    string       path;
    vector<char> data;
    string       mime_type; // 静态文件类型
};

map<string, FileInfo> fileMap;

map<string, string> fileMineTypeMap = {
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif", "image/gif"},
    {".ico", "image/x-icon"},
};

void getMimeTypeByPath(const string& path, string& mime_type) {

    // 获取文件后缀名
    string suffix = path.substr(path.find_last_of('.'));

    // 根据后缀名获取文件类型
    if (fileMineTypeMap.find(suffix) != fileMineTypeMap.end()) {
        mime_type = fileMineTypeMap[suffix];
    } else {
        mime_type = "text/plain";
    }
}

int addStaticFileToMap(const string& filepath, unsigned char* data, size_t size) {
    FileInfo fileInfo;
    string   path;
    string   tmpPath = filepath;

    if (data == nullptr || size <= 0) {
        logE("addStaticFileToMap failed, data[%p] size[%zu]", data, size);
        return -1;
    }

    // 将路径中的反斜杠替换为斜杠
    for (auto& c : tmpPath) {
        if (c == '\\') {
            c = '/';
        }
    }

    // 去除文件路径的前缀
    path = tmpPath;
    if (path.find(FILE_PREFIX) == 0) {
        path = path.substr(strlen(FILE_PREFIX));
    }

    fileInfo.path = path;
    fileInfo.data.assign(data, data + size);
    getMimeTypeByPath(path, fileInfo.mime_type);
    fileMap[path] = fileInfo;
    logD("map file path: %s", path.c_str());

    return 0;
}

void* loadEmbedFile(int resourceID, DWORD& size) {
    // 获取当前模块的句柄
    HMODULE hModule = GetModuleHandle(nullptr);
    if (!hModule) {
        return nullptr;
    }

    // 查找资源
    HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(resourceID), RT_RCDATA);
    if (!hRes) {
        return nullptr;
    }

    // 获取资源大小
    size = SizeofResource(hModule, hRes);
    if (size == 0) {
        return nullptr;
    }

    // 加载资源
    HGLOBAL hData = LoadResource(hModule, hRes);
    if (!hData) {
        return nullptr;
    }

    // 锁定资源并返回指向资源数据的指针
    return LockResource(hData);
}

int initStaticFile() {
    unsigned char* zip_file_data = nullptr;
    DWORD          zip_file_size = 0;
    int            fileIds[]     = {IDR_FILE1, IDR_FILE2};

    for (auto id : fileIds) {
        // 加载rc文件的IDR_FILE1文件资源到内存
        zip_file_data = (unsigned char*)loadEmbedFile(id, zip_file_size);
        if (!zip_file_data) {
            logE("load web zip file failed!");
            return -1;
        }
        logD("load web zip file success, size[%lu]", zip_file_size);

        // 解压 并构建文件索引
        if (0 != decompressZipData(
                     (const unsigned char*)zip_file_data, zip_file_size, addStaticFileToMap)) {
            logE("decompress zip file failed!");
            return -1;
        }
    }

    return 0;
}

int staticFileGetFile(const std::string& path, std::vector<char>& data, string& mime_type) {
    string tmpPath = path;

    // 将路径中的反斜杠替换为斜杠
    for (auto& c : tmpPath) {
        if (c == '\\') {
            c = '/';
        }
    }

    // 如果路径以 / 开头，去掉 /
    if (tmpPath[0] == '/') {
        tmpPath = tmpPath.substr(1);
    }

    // 如果路径以 / 结尾，去掉 /
    if (tmpPath[tmpPath.size() - 1] == '/') {
        tmpPath = tmpPath.substr(0, tmpPath.size() - 1);
    }

    auto it = fileMap.find(tmpPath);
    if (it == fileMap.end()) {
        return -1;
    }
    data      = it->second.data;
    mime_type = it->second.mime_type;
    return 0;
}
