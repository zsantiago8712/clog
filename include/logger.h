#pragma once

typedef enum out_type {

    TERMINAL_OUT,
    FILE_OUT,
    ALL_OUT,

} OutType;

void initLogger(OutType type);
void terminateLogger(void);
