#include "logger.h"
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_MEESAGE_BUFFER_SIZE 256
#define MAX_TIME_BUFFER_SIZE 64

typedef enum {

    LEVEL_DEBUG = 0,
    LEVEL_INFO = 1,
    LEVEL_WARNING = 2,
    LEVEL_ERROR = 3,
    LEVEL_FATAL = 4,

    LOG_MAX
} Log_Leves;

typedef struct {
    FILE* log_files[LOG_MAX];
    pthread_mutex_t lock;
    char time_buffer[MAX_TIME_BUFFER_SIZE];
    time_t last_time;

} Logger;

static void formatTime(void);
static const char* getFormatByLogLevel(Log_Leves level);

static Logger logger = {
    .log_files = {NULL, NULL, NULL, NULL, NULL},
};

#define WITH_COLORS

void initLogger(OutType type) {
    const char* files[] = {
        "DEBUG.log", "INFO.log", "WARNING.log", "ERROR.log", "FATAL.log",
    };
    for (int i = 0; i < LOG_MAX; i++) {
        logger.log_files[i] = fopen(files[i], "w+");
        if (logger.log_files[i] == NULL) {
            terminateLogger();
            exit(-1);
        }
    }

    switch (type) {
        case TERMINAL_OUT:
            break;
        case FILE_OUT:
            break;
        case ALL_OUT:
            break;
        default:
            break;
    }
    pthread_mutex_init(&logger.lock, NULL);
}

void terminateLogger(void) {
    for (int i = 0; i < LOG_MAX; i++) {
        if (logger.log_files[i] != NULL) {
            fclose(logger.log_files[i]);
        }
    }
    pthread_mutex_destroy(&logger.lock);
}

// HACK: Enconrtrear una mejora maneera de implementar los log levels, puede ser
// que leo mejor sea ponerlos en el header
void logMessage(int level,
                const int line,
                const char* file,
                const char* format,
                ...) {
    pthread_mutex_lock(&logger.lock);

#ifdef WITH_COLORS
    /*const char* levels[] = {"\x1b[36mDEBUG\x1b[0m", "\x1b[32mINFO\x1b[0m",
                            "\x1b[33mWARNING\x1b[0m", "\x1b[31mERROR\x1b[0m",
                            "\x1b[35mFATAL\x1b[0m"};*/
#else
    const char* levels[] = {
        "DEBUG", "INFO", "WARNING", "ERROR", "FATAL",
    };
#endif
    va_list args;

    formatTime();
    char log_buffer[MAX_MEESAGE_BUFFER_SIZE];

    va_start(args, format);
    vsnprintf(log_buffer, sizeof(log_buffer), format, args);
    va_end(args);

    fprintf(stderr, getFormatByLogLevel((Log_Leves)level), file, line,
            logger.time_buffer, log_buffer);
    pthread_mutex_unlock(&logger.lock);
    fflush(stderr);
}

static const char* getFormatByLogLevel(Log_Leves level) {
    switch (level) {
        case LEVEL_DEBUG:
            return "[DEBUG] [File: %s] [Line: %d] [Time: %s] -> %s\n";

        case LEVEL_INFO:
            return "[INFO] [Line: %d] [File: %s] [Time: %s] -> %s\n";

        case LEVEL_WARNING:
            return "[WARNING] [Line: %d] [File: %s] [Time: %s] -> \n";
        case LEVEL_ERROR:
            return "[ERROR] [Line: %d] [File: %s] [Time: %s] -> \n";

        case LEVEL_FATAL:
            return "[FATAL] [Line: %d] [File: %s] [Time: %s] -> \n";

        case LOG_MAX:
            return "[Line: %d] [File: %s] [Time: %s] -> \n";
    }

    return "[Line: %d] [File: %s] [Time: %s] -> \n";
}

// -------------------STATIC FUNCTIONS--------------------
static void formatTime(void) {
    time_t time_now = time(0);

    if (time_now != logger.last_time) {
        struct tm* time = localtime(&time_now);
        strftime(logger.time_buffer, MAX_TIME_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S",
                 time);
        logger.last_time = time_now;
    }
}

// static void outputInTerminal(const char* buffer) {
//     fprintf(stderr, "%s", buffer);
// }
//
// static void outputInFile(const char* buffer) {
//     fprintf(logger.log_files[LOG_DEBUG], "%s", buffer);
// }
