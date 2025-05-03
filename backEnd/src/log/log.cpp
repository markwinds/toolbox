#include "log.h"
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <string>
#include <thread>

using namespace std;

// 全局日志级别，默认为WARN
static LogLevel g_logLevel = LOG_LEVEL_WARN;

// 获取当前日志级别
LogLevel getLogLevel() {
    return g_logLevel;
}

// 设置当前日志级别
void setLogLevel(LogLevel level) {
    g_logLevel = level;
}

// 检查给定的日志级别是否应该被记录
bool shouldLog(LogLevel level) {
    return level >= g_logLevel;
}

std::string getFileName(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    return (pos == std::string::npos) ? path : path.substr(pos + 1);
}

void log(
    const LogLevel& level, const char* file, const char* func, int line, const char* format, ...) {
    // 检查日志级别 (这里是双重检查，宏定义中已经检查过一次)
    if (!shouldLog(level)) {
        return;
    }

    va_list args;
    va_start(args, format);
    char tmp[2048]       = {0};
    int  write_len       = 0;
    char log_level_str[] = "DIWEF";

    // 文件名只保留文件名 兼容linux和windows
    string file_name = getFileName(file);

    // 获取当前时间
    time_t  now = time(nullptr);
    std::tm tm_info{};
    //    tm_info = *std::localtime(&now);
    localtime_s(&tm_info, &now);

    // 获取线程ID
    std::thread::id tid = std::this_thread::get_id();

    // 格式化输出
    write_len += snprintf(tmp + write_len,
                          sizeof(tmp) - write_len,
                          "[%c][%04d-%02d-%02d %02d:%02d:%02d][%s:%s:%d][tid:%x] ",
                          log_level_str[level],
                          tm_info.tm_year + 1900,
                          tm_info.tm_mon + 1,
                          tm_info.tm_mday,
                          tm_info.tm_hour,
                          tm_info.tm_min,
                          tm_info.tm_sec,
                          file_name.c_str(),
                          func,
                          line,
                          tid);

    write_len += vsnprintf(tmp + write_len, sizeof(tmp) - write_len, format, args);
    va_end(args);
    write_len += snprintf(tmp + write_len, sizeof(tmp) - write_len, "\n\n");

    // 写入日志文件
    FILE* fp = fopen("log.txt", "ab");
    if (fp) {
        fwrite(tmp, 1, write_len, fp);
        fclose(fp);
    }
}
