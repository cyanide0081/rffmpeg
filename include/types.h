#ifndef H_TYPES
#define H_TYPES

#include "libs.h"
#include "constants.h"

#include <stdarg.h>

typedef enum inputMode {
    ARGUMENTS, CONSOLE
} inputMode;

typedef struct processInfo {
    size_t convertedFiles;
    size_t deletedFiles;
    double executionTime;
} processInfo;

typedef struct fmtTime {
    uint64_t hours;
    uint64_t minutes;
    double seconds;
} fmtTime;

typedef struct arguments {
    char **inPaths;
    char **inFormats;
    char *ffOptions;
    char *outFormat;

    union {
        char *customFolderName;
        char *customPathName;
    };

    uint8_t options; // Bit fields for the optional arguments
} arguments;

arguments *initializeArguments(void);

void destroyArguments(arguments *arguments);

fmtTime formatTime(double seconds);

void trimSpaces(char *string);

void *xcalloc(size_t numberOfElements, size_t sizeOfElements);

char *asprintf(const char *format, ...);

#endif // H_TYPES