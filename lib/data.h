#ifndef H_TYPES
#define H_TYPES

#include <libs.h>
#include <constants.h>
#include <stdarg.h>

#ifdef _WIN32
#include <win.h>
#endif

/* NOTE: this'll probably be obsolete soon */
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

#define printErr(msg, dsc)                      \
    fprintf(                                    \
            stderr,                             \
            "%s ERROR: %s%s (%s%s%s)\n\n",      \
            CHARCOLOR_RED,                      \
            CHARCOLOR_WHITE, msg,               \
            CHARCOLOR_RED, dsc,                 \
            COLOR_DEFAULT)

#ifndef _WIN32

#define strerror_s(buf, bufsz, errno) strerror_r(errno, buf, bufsz)

#endif

arguments *initializeArguments(void);

void destroyArguments(arguments *arguments);

fmtTime formatTime(double seconds);

void trimSpaces(char *string);

char *trimUTF8StringTo(const char *str, size_t maxChars);

void *xcalloc(size_t numberOfElements, size_t sizeOfElements);

char *_asprintf(const char *format, ...);

#endif // H_TYPES
