#ifndef H_TYPES
#define H_TYPES

typedef struct processInformation {
    uint64_t convertedFiles;
    uint64_t deletedFiles;
    double executionTime;
} processInfo_t;

typedef enum inputMode {
    ARGUMENTS, CONSOLE
} inputMode_t;

typedef enum errorCode {
    ERROR_NONE,
    ERROR_FAILED_SETCONSOLEMODE,
    ERROR_FAILED_TO_OPEN_DIRECTORY,
    ERROR_NO_INPUT_FORMAT,
    ERROR_NO_OUTPUT_FORMAT
} errorCode_t;

#endif