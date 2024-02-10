#include "logger.h"
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_MEESAGE_BUFFER_SIZE 256
#define MAX_TIME_BUFFER_SIZE 64

typedef enum {

    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_FATAL = 4,

    LOG_MAX
} Log_Leves;

typedef struct {
    FILE* log_files[LOG_MAX];
    char* format;
    int line;
    char* file;
    pthread_mutex_t lock;
} Logger;

static void formatMesage(char* message_buffer, va_list args);
static void formatTime(char* time_buffer);
static void formatLogg(Log_Leves level,
                       char* buffer,
                       char* time,
                       char* message);

static Logger logger = {.log_files = {NULL, NULL, NULL, NULL, NULL},
                        .locked = 0};

#ifdef WITH_COLORS
static const char* levels[] = {"\x1b[36mDEBUG\x1b[0m", "\x1b[32mINFO\x1b[0m",
                               "\x1b[33mWARN\x1b[0m", "\x1b[31mERROR\x1b[0m",
                               "\x1b[35mFATAL\x1b[0m"};
#else
static const char* levels[] = {
    "DEBUG", "INFO", "WARNING", "ERROR", "FATAL",
};
#endif

void initLogger(OutType type) {
    char file[15];

    for (int i = 0; i < LOG_MAX; i++) {
        snprintf(file, sizeof(file), "%s.log", levels[i]);

        logger.log_files[i] = fopen(file, "w+");
        if (logger.log_files[i] == NULL) {
            terminateLogger();
            exit(-1);
        }
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

void logMessage(Log_Leves level,
                const int line,
                char* file,
                char* format,
                ...) {
    pthread_mutex_lock(&logger.lock);

    char buffer[MAX_BUFFER_SIZE];
    char message_buffer[256];
    char time_buffer[64];
    va_list args;

    va_start(args, format);
    logger.format = format;
    logger.line = line;
    logger.file = file;

    formatMesage(message_buffer, args);
    va_end(args);

    formatTime(time_buffer);
    formatLogg(level, buffer, time_buffer, message_buffer);
    printf("%s", buffer);

    pthread_mutex_unlock(&logger.lock);
}

// -------------------STATIC FUNCTIONS--------------------

static void formatMesage(char* message_buffer, va_list args) {
    const int message_size =
        vsnprintf(message_buffer, MAX_MEESAGE_BUFFER_SIZE, logger.format, args);

    if (message_size > MAX_MEESAGE_BUFFER_SIZE) {
        message_buffer[MAX_MEESAGE_BUFFER_SIZE - 1] = '\0';
    }
}

static void formatTime(char* time_buffer) {
    time_t time_now = time(0);
    struct tm* time = localtime(&time_now);

    const size_t time_size =
        strftime(time_buffer, MAX_TIME_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", time);

    if (time_size >= MAX_TIME_BUFFER_SIZE) {
        time_buffer[MAX_TIME_BUFFER_SIZE - 1] = '\0';
    } else if (time_size < MAX_TIME_BUFFER_SIZE && time_size > 0) {
        time_buffer[time_size] = '\0';
    }
}

static void formatLogg(Log_Leves level,
                       char* buffer,
                       char* time,
                       char* message) {
    buffer[snprintf(buffer, MAX_BUFFER_SIZE,
                    "[%s] [Line: %d] [File: %s]: %s %s\n", levels[(int)level],
                    logger.line, logger.file, message, time)] = '\0';
}

static void outputInTerminal(const char* buffer) {
    fprintf(stderr, "%s", buffer);
}

static void outputInFile(const char* buffer) {
    fprintf(logger.log_files[LOG_DEBUG], "%s", buffer);
}
