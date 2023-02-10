#ifndef H_TYPES_PRIV
#define H_TYPES_PRIV

#include "libs.h"

typedef enum inputMode {
    ARGUMENTS, CONSOLE
} inputMode;

typedef struct {
    size_t convertedFiles;
    size_t deletedFiles;
    double executionTime;
} processInfo;

typedef struct {
    uint64_t hours;
    uint64_t minutes;
    double seconds;
} formattedTime;

typedef struct {
    char16_t *inputPaths[SHORTBUF];
    char16_t *inputFormats[SHORTBUF];
    char16_t ffmpegOptions[BUFFER];
    char16_t outputFormat[SHORTBUF];

    union {
        char16_t customFolderName[PATHBUF];
        char16_t customPathName[PATHBUF];
    };

    size_t inputPathsCount;
    size_t inputFormatsCount;

    uint16_t options; // Bit fields for the optional arguments
} arguments;

#endif