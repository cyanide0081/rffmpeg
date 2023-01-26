#ifndef H_TYPES
#define H_TYPES

#include "libs.h"

typedef struct processInformation {
    uint64_t convertedFiles;
    uint64_t deletedFiles;
    double executionTime;
} processInfo_t;

typedef enum inputMode { ARGUMENTS, CONSOLE } inputMode_t;

#endif