#pragma once

typedef enum out_type {

    TERMINAL_OUT,
    FILE_OUT,
    ALL_OUT,

} OutType;

void initLogger(OutType type);
void terminateLogger(void);
void logMessage(int level,
                const int line,
                const char* file,
                const char* format,
                ...);

#define LOG_DEBUG(format, ...) \
    logMessage(0, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define LOG_WARNING(format, ...) \
    logMessage(0, __LINE__, __FILE__, format, ##__VA_ARGS__)
