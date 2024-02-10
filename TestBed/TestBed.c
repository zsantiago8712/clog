#include "logger.h"

// #include <time.h>
#include <stdio.h>
#include <sys/time.h>

int main(int argc, char* argv[]) {
    initLogger(TERMINAL_OUT);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < 1000000; i++) {
        LOG_DEBUG("Hola mundo %i", i);
    }

    gettimeofday(&end, NULL);
    long seconds = (end.tv_sec - start.tv_sec);
    long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

    printf("SE TARDO %ld.%ld\n", seconds, micros);
    terminateLogger();
    return 0;
}
