#include "logger.h"
#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_MEESAGE_BUFFER_SIZE 256
#define MAX_TIME_BUFFER_SIZE 64
#define MAX_QUEUE_LOGS 32

typedef enum {

    LEVEL_DEBUG = 0,
    LEVEL_INFO = 1,
    LEVEL_WARNING = 2,
    LEVEL_ERROR = 3,
    LEVEL_FATAL = 4,

    LOG_MAX
} Log_Leves;

typedef struct {
    pthread_mutex_t lock;
    char time_buffer[MAX_TIME_BUFFER_SIZE];
    time_t last_time;
    int its_initialized;

    char queue_logs[MAX_QUEUE_LOGS][MAX_BUFFER_SIZE];

    int logs_in_queue;
    OutType type_flag;

    FILE* output;
    FILE* log_file;

} Logger;

static void formatTime(void);
static const char* getFormatByLogLevel(Log_Leves level);
static const char* getFormatByLogLevelForFile(Log_Leves level);

static Logger logger = {
    .output = NULL,
    .its_initialized = 0,
    .log_file = NULL,
    .logs_in_queue = 0,
};

void initLogger(OutType type_flag) {
    if (!logger.its_initialized) {
        pthread_mutex_init(&logger.lock, NULL);
        logger.its_initialized = 1;
        logger.type_flag = type_flag;

        switch (type_flag) {
            case STDOUT:
                logger.output = stdout;
                break;
            case STDERR:
                logger.output = stderr;
                break;
            case L_FILE:
                setLogFile("clog.log");
                break;
            case ALL:
                logger.output = stderr;
                setLogFile("clog.log");
                break;
            default:
                break;
        }
    }
}

void setLogFile(const char* file_name) {
    if (logger.log_file != NULL) {
        fclose(logger.log_file);
    }

    logger.log_file = fopen(file_name, "w");
    if (logger.log_file == NULL) {
        perror("fopen");
        terminateLogger();
        exit(-1);
    }
}

void terminateLogger(void) {
    if (logger.its_initialized) {
        if (logger.output != NULL) {
            fclose(logger.output);
        }
        logger.its_initialized = 0;
        pthread_mutex_destroy(&logger.lock);
    }
}

// HACK: Enconrtrear una mejora maneera de implementar los log levels, puede ser
// que leo mejor sea ponerlos en el header
void logMessage(int level,
                const int line,
                const char* file,
                const char* format,
                ...) {
    pthread_mutex_lock(&logger.lock);

    va_list args;
    char log_buffer[MAX_MEESAGE_BUFFER_SIZE];

    va_start(args, format);
    vsnprintf(log_buffer, sizeof(log_buffer), format, args);
    va_end(args);

    formatTime();

    if (logger.type_flag == ALL) {
        assert(logger.log_file != NULL);

        snprintf(logger.queue_logs[logger.logs_in_queue++], MAX_BUFFER_SIZE,
                 getFormatByLogLevelForFile((Log_Leves)level), file, line,
                 logger.time_buffer, log_buffer);

        if (logger.logs_in_queue == MAX_QUEUE_LOGS) {
            for (int i = 0; i < MAX_QUEUE_LOGS; i++) {
                fprintf(logger.log_file, "%s", logger.queue_logs[i]);
                logger.logs_in_queue--;
            }
        }

        fprintf(logger.output, getFormatByLogLevel((Log_Leves)level), file,
                line, logger.time_buffer, log_buffer);

    } else if (logger.type_flag == L_FILE) {
        assert(logger.log_file != NULL);

        snprintf(logger.queue_logs[logger.logs_in_queue++], MAX_BUFFER_SIZE,
                 getFormatByLogLevelForFile((Log_Leves)level), file, line,
                 logger.time_buffer, log_buffer);

        if (logger.logs_in_queue == MAX_QUEUE_LOGS) {
            for (int i = 0; i < MAX_QUEUE_LOGS - 1; i++) {
                fprintf(logger.log_file, "%s", logger.queue_logs[i]);
                logger.logs_in_queue--;
            }
        }
    } else {
        fprintf(logger.output, getFormatByLogLevel((Log_Leves)level), file,
                line, logger.time_buffer, log_buffer);
    }

    pthread_mutex_unlock(&logger.lock);
    fflush(stderr);
}

#define WITH_COLORS

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

static const char* getFormatByLogLevel(Log_Leves level) {
    switch (level) {
        case LEVEL_DEBUG:
#ifdef WITH_COLORS
            return "\x1b[36m[DEBUG]\x1b[0m[File: %s] [Line: %d] [Time: %s] -> "
                   "%s\n";
#else
            return "[DEBUG] [File: %s] [Line: %d] [Time: %s] -> %s\n";
#endif
        case LEVEL_INFO:
#ifdef WITH_COLORS
            return "\x1b[32m[INFO] [File: %s] [Line: %d] [Time: %s] -> "
                   "%s\x1b[0m\n";
#else
            return "[INFO] [Line: %d] [File: %s] [Time: %s] -> %s\n";
#endif
        case LEVEL_WARNING:
#ifdef WITH_COLORS
            return "\x1b[33m[WARNING] [File: %s] [Line: %d] [Time: %s] -> "
                   "%s\x1b[0m\n";
#else
            return "[WARNING] [Line: %d] [File: %s] [Time: %s] -> \n";
#endif
        case LEVEL_ERROR:
#ifdef WITH_COLORS
            return "\x1b[31m[ERROR] [File: %s] [Line: %d] [Time: %s] -> "
                   "%s\x1b[0m\n";
#else
            return "[ERROR] [Line: %d] [File: %s] [Time: %s] -> \n";
#endif
        case LEVEL_FATAL:
#ifdef WITH_COLORS
            return "\x1b[35m[FATAL]\x1b[0m [File: %s] [Line: %d] [Time: %s] -> "
                   "%s\n";
#else
            return "[FATAL] [Line: %d] [File: %s] [Time: %s] -> \n";
#endif
        case LOG_MAX:
            return "[Line: %d] [File: %s] [Time: %s] -> \n";
    }

    return "[Line: %d] [File: %s] [Time: %s] -> \n";
}

static const char* getFormatByLogLevelForFile(Log_Leves level) {
    switch (level) {
        case LEVEL_DEBUG:
            return "[DEBUG] [File: %s] [Line: %d] [Time: %s] -> %s\n";
        case LEVEL_INFO:
            return "[INFO] [File: %s] [Line: %d] [Time: %s] -> %s\n";
        case LEVEL_WARNING:
            return "[WARNING] [File: %s] [Line: %d] [Time: %s] -> %s\n";
        case LEVEL_ERROR:
            return "[ERROR] [File: %s] [Line: %d] [Time: %s] -> %s\n";
        case LEVEL_FATAL:
            return "[FATAL] [File: %s] [Line: %d] [Time: %s] -> %s\n";
        case LOG_MAX:
            return "[File: %s] [Line: %d] [Time: %s] -> %s\n";
    }

    return "[File: %s] [Line: %d] [Time: %s] -> %s\n";
}
