#pragma once

enum LogLevel {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO  = 1,
    LOG_LEVEL_WARN  = 2,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_FATAL = 4,
};

void log(
    const LogLevel& level, const char* file, const char* func, int line, const char* format, ...)
    __attribute__((format(scanf, 5, 6)));

// 日志 自动记录时间、文件名、函数名、行号
#define logI(format, ...) \
    log(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#define logD(format, ...) \
    log(LOG_LEVEL_DEBUG, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#define logW(format, ...) \
    log(LOG_LEVEL_WARN, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#define logE(format, ...) \
    log(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#define logF(format, ...) \
    log(LOG_LEVEL_FATAL, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
