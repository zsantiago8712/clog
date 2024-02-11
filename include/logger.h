#pragma once

typedef enum {

    LEVEL_DEBUG = 0,
    LEVEL_INFO = 1,
    LEVEL_WARNING = 2,
    LEVEL_ERROR = 3,
    LEVEL_FATAL = 4,

    LOG_MAX
} Log_Level;

typedef enum out_type {

    STDOUT,
    STDERR,
    L_FILE,
    ALL,

} OutType;

void initLogger(OutType type_flag);
void terminateLogger(void);
void logMessage(Log_Level level,
                const int line,
                const char* file,
                const char* format,
                ...);

void setLogFile(const char* file_name);
#define LOG_DEBUG(format, ...) \
    logMessage(LEVEL_DEBUG, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define LOG_WARNING(format, ...) \
    logMessage(LEVEL_WARNING, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define LOG_INFO(format, ...) \
    logMessage(LEVEL_INFO, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) \
    logMessage(LEVEL_ERROR, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define LOG_FATAL(format, ...) \
    logMessage(LEVEL_FATAL, __LINE__, __FILE__, format, ##__VA_ARGS__)
