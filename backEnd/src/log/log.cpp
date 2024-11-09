//
// Created by 14640 on 2024/11/5.
//

#include <cstdarg>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <string>
#include "log.h"

using namespace std;


std::string getFileName(const std::string &path) {
    size_t pos = path.find_last_of("/\\");
    return (pos == std::string::npos) ? path : path.substr(pos + 1);
}


void log(const LogLevel &level, const char *file, const char *func, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    char tmp[2048] = {0};
    int write_len = 0;
    char log_level_str[] = "DIWEF";

    // 文件名只保留文件名 兼容linux和windows
    string file_name = getFileName(file);

    // 获取当前时间
    time_t now = time(nullptr);
    std::tm tm_info;
    //    tm_info = *std::localtime(&now);
    localtime_s(&tm_info, &now);

    // 格式化输出
    write_len += snprintf(tmp + write_len, sizeof(tmp) - write_len, "[%c][%04d-%02d-%02d %02d:%02d:%02d][%s:%s:%d] ",
                          log_level_str[level], tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
                          tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec,
                          file_name.c_str(), func, line);

    write_len += vsnprintf(tmp + write_len, sizeof(tmp) - write_len, format, args);
    va_end(args);
    write_len += snprintf(tmp + write_len, sizeof(tmp) - write_len, "\n\n");

    // 写入日志文件
    FILE *fp = fopen("log.txt", "ab");
    if (fp) {
        fwrite(tmp, 1, write_len, fp);
        fclose(fp);
    }
}
