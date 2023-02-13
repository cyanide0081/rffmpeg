#ifndef H_TYPES_PRIV
#define H_TYPES_PRIV

#include "libs.h"
#include "constants.h"

typedef enum inputMode {
    ARGUMENTS, CONSOLE
} inputMode;

typedef struct processInfo {
    size_t convertedFiles;
    size_t deletedFiles;
    double executionTime;
} processInfo;

typedef struct formattedTime {
    uint64_t hours;
    uint64_t minutes;
    double seconds;
} formattedTime;

typedef struct arguments {
    char16_t *inputPaths[SHORTBUF];
    char16_t *inputFormats[SHORTBUF];
    char16_t ffmpegOptions[BUFFER];
    char16_t outputFormat[SHORTBUF];

    union {
        char16_t customFolderName[PATH_BUFFER];
        char16_t customPathName[PATH_BUFFER];
    };

    size_t inputPathsCount;
    size_t inputFormatsCount;

    uint16_t options; // Bit fields for the optional arguments
} arguments;

extern arguments *initializeArguments(void);

extern void destroyArguments(arguments *arguments);

extern formattedTime formatTime(double seconds);

extern void trimWhiteSpaces(char16_t *string);

#endif