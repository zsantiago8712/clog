#pragma once

typedef enum out_type {

    STDOUT,
    STDERR,
    L_FILE,
    ALL,

} OutType;

void initLogger(OutType type_flag);
void terminateLogger(void);
void logMessage(int level,
                const int line,
                const char* file,
                const char* format,
                ...);

void setLogFile(const char* file_name);
#define LOG_DEBUG(format, ...) \
    logMessage(0, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define LOG_WARNING(format, ...) \
    logMessage(0, __LINE__, __FILE__, format, ##__VA_ARGS__)
