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

#define FMT_BUF 32

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
    char *ffOptions;
    char **inFormats;
    const char *outFormat;

    union {
        char *customFolder;
        char *customPath;
    };

    uint8_t options; // Bit fields for the optional arguments
} arguments;

#define PROGRAM_VERSION      "v1.1.0"
#define CONSOLE_WINDOW_TITLE ("RFFmpeg " PROGRAM_VERSION)

/* ANSI escape chars for colored shell output */

#ifdef _WIN32
#define COLOR_DEFAULT        "\x1b[0m"
#define COLOR_ACCENT         "\x1b[91m"
#define COLOR_INPUT          "\x1b[97m"
#define COLOR_ERROR          "\x1b[91m"
#else
#define COLOR_DEFAULT        "\033[0m"
#define COLOR_ACCENT         "\033[91m"
#define COLOR_INPUT          "\033[97m"
#define COLOR_ERROR          "\033[91m"
#endif

#define LIST_BUF 16

#ifdef _WIN32
#define FILE_BUF MAX_PATH
#define ARG_BUF  SHRT_MAX /* max 'Unicode_String' size on Windows */
#else
#define FILE_BUF NAME_MAX
#define ARG_BUF  ARG_MAX
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
            "%s ERROR: %s%s: %s\"%s\"%s\n\n",      \
            COLOR_ERROR,                        \
            COLOR_DEFAULT, msg,               \
            COLOR_INPUT, dsc,                 \
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

arguments *allocArguments(void);

void freeArguments(arguments *arguments);

fmtTime formatTime(double seconds);

void trimSpaces(char *string);

char *trimUTF8StringTo(const char *str, size_t maxChars);

void *xcalloc(size_t numberOfElements, size_t sizeOfElements);

char *_asprintf(const char *format, ...);

void readLine(char *dst, size_t dstSize);

bool isDirectory(const char *dir);

#endif // H_TYPES
