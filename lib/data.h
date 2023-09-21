#ifndef H_TYPES
#define H_TYPES

#ifdef __linux__
#include <linux/limits.h>
#endif

#include <libs.h>
#include <stdarg.h>

#ifdef _WIN32
#include <win.h>
#endif

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


#define PROGRAM_NAME    "RFFMPEG"
#define PROGRAM_VERSION "v1.1.0"
#define FULL_PROGRAM_TITLE   (PROGRAM_NAME " " PROGRAM_VERSION " (跨平台)")
#define CONSOLE_WINDOW_TITLE (PROGRAM_NAME " " PROGRAM_VERSION)

/* ANSI escape chars for colored shell output */
#define COLOR_DEFAULT        "\x1b[0m"
#define CHARCOLOR_RED        "\x1b[31m"
#define CHARCOLOR_WHITE      "\x1b[37m"
#define CHARCOLOR_RED_BOLD   "\x1b[91m"
#define CHARCOLOR_WHITE_BOLD "\x1b[97m"

#define LIST_BUF 16

#ifdef _WIN32
#define FILE_BUF MAX_PATH
#else
#define FILE_BUF NAME_MAX
#endif

#ifndef NDEBUG
#define dprintf(fmt, ...) fprintf(stderr, "%s:%d:%s(): " fmt, \
                                  __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define dprintf(...) do {} while (false)
#endif

#define printErr(msg, dsc)                      \
    fprintf(                                    \
            stderr,                             \
            "%s ERROR: %s%s (%s%s%s)\n\n",      \
            CHARCOLOR_RED,                      \
            CHARCOLOR_WHITE, msg,               \
            CHARCOLOR_RED, dsc,                 \
            COLOR_DEFAULT)

#define xrealloc(buf, size) do {                        \
        void *tmp = realloc(buf, size);                 \
        if (!tmp) {                                     \
            free(buf);                                  \
            printErr("out of memory", strerror(errno)); \
            exit(errno);                                \
        }                                               \
                                                        \
        buf = tmp;                                      \
    } while (false)                                     \

arguments *initializeArguments(void);

void destroyArguments(arguments *arguments);

fmtTime formatTime(double seconds);

void trimSpaces(char *string);

char *trimUTF8StringTo(const char *str, size_t maxChars);

void *xcalloc(size_t numberOfElements, size_t sizeOfElements);

char *_asprintf(const char *format, ...);

ssize_t getline(char **string, size_t *buffer, FILE *stream);

#endif // H_TYPES
