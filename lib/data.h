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

#define PROGRAM_VERSION      "v1.1.1"
#define CONSOLE_WINDOW_TITLE ("RFFmpeg " PROGRAM_VERSION)

/* ANSI escape chars for colored shell output */
#ifndef NO_ANSI_ESCAPE_CODES
#define COLOR_DEFAULT        "\x1b[0m"
#define COLOR_ACCENT         "\x1b[91m"
#define COLOR_INPUT          "\x1b[97m"
#define COLOR_ERROR          "\x1b[91m"
#else
#define COLOR_DEFAULT
#define COLOR_ACCENT
#define COLOR_INPUT
#define COLOR_ERROR
#endif

#define LIST_BUF 8

#ifdef _WIN32
#define FILE_BUF MAX_PATH
#define ARG_BUF  SHRT_MAX /* max 'Unicode_String' size on Windows */
#define PATH_SEP '\\'
#else
#define FILE_BUF NAME_MAX
#define ARG_BUF  ARG_MAX
#define PATH_SEP '/'
#endif

/* FIXME: skips user input if a conversion succeeded */
#ifdef _WIN32
#define _waitForNewLine()                                           \
    wint_t c = getwchar();                                          \
    ungetwc(c, stdin);                                              \
    while ((c = getwchar()) != u'\n' && c != u'\r' && c != WEOF)
#else
#define _waitForNewLine()                               \
    int c;                                              \
    while ((c = getchar()) != '\n' && c != EOF);        \
    getchar()
#endif

#ifndef NDEBUG
#define dprintf(fmt, ...) fprintf(stderr, "%s:%d:%s(): " fmt,           \
                                  __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define dprintf(...) do {} while (false)
#endif

#define printErr(msg, dsc)                      \
    fprintf(                                    \
            stderr,                             \
            "%s ERROR: %s%s: %s\"%s\"%s\n\n",   \
            COLOR_ERROR,                        \
            COLOR_DEFAULT, msg,                 \
            COLOR_INPUT, dsc,                   \
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
