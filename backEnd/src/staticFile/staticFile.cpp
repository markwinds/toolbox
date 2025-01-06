//
// Created by 14640 on 2024/11/7.
//

#include "staticFile.h"
#include "log.h"
#include "rc.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <windows.h>
#include <zip.h>

#define FILE_PREFIX "dist/"

using namespace std;

struct FileInfo {
    string       path;
    vector<char> data;
    string       mime_type; // 静态文件类型
};

map<string, FileInfo> file_map;

map<string, string> file_mine_type_map = {
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif", "image/gif"},
    {".ico", "image/x-icon"},
};

void get_mime_type_by_path(const string& path, string& mime_type) {

    // 获取文件后缀名
    string suffix = path.substr(path.find_last_of('.'));

    // 根据后缀名获取文件类型
    if (file_mine_type_map.find(suffix) != file_mine_type_map.end()) {
        mime_type = file_mine_type_map[suffix];
    } else {
        mime_type = "text/plain";
    }
}

int add_static_file_to_map(const string& filepath, unsigned char* data, size_t size) {
    FileInfo fileInfo;
    string   path;
    string   tmpPath = filepath;

    if (data == nullptr || size <= 0) {
        logE("add_static_file_to_map failed, data[%p] size[%d]", data, size);
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
    get_mime_type_by_path(path, fileInfo.mime_type);
    file_map[path] = fileInfo;
    logD("map file path: %s", path.c_str());

    return 0;
}

int decompress_zip_data(const unsigned char* compressed_data, size_t compressed_length) {
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
            logE("malloc failed, size[%d]", stat.size);
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
        printf("条目 %s 解压成功，大小: %lu\n", entry_name, stat.size);

        add_static_file_to_map(entry_name, output_data, stat.size);

        // 关闭文件
        zip_fclose(file);

        // 释放内存
        free(output_data);
    }

    // 成功
    ret = 0;

exit:
    zip_error_fini(&error);

    //    if (0 != ret) {
    //        if (src) {
    //            zip_source_free(src);
    //            src = nullptr;
    //        }
    //        if (za) {
    //            zip_close(za);
    //            za = nullptr;
    //        }
    //        free((void *) compressed_data);
    //    }

    return ret;
}

void* load_embed_file(int resourceID, DWORD& size) {
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

int init_static_file() {
    unsigned char* zip_file_data = nullptr;
    DWORD          zip_file_size = 0;

    // 加载rc文件的IDR_FILE1文件资源到内存
    zip_file_data = (unsigned char*)load_embed_file(IDR_FILE1, zip_file_size);
    if (!zip_file_data) {
        logE("load web zip file failed!");
        return -1;
    }
    logD("load web zip file success, size[%d]", zip_file_size);

    // 解压 并构建文件索引
    if (0 != decompress_zip_data((const unsigned char*)zip_file_data, zip_file_size)) {
        logE("decompress zip file failed!");
        return -1;
    }

    return 0;
}

int static_file_get_file(const std::string& path, std::vector<char>& data, string& mime_type) {
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

    auto it = file_map.find(tmpPath);
    if (it == file_map.end()) {
        return -1;
    }
    data      = it->second.data;
    mime_type = it->second.mime_type;
    return 0;
}
