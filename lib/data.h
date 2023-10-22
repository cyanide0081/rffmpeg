#ifndef H_TYPES
#define H_TYPES

#include <libs.h>
#include <arena.h>

#ifdef _WIN32
#include <win.h>
#endif

#define FMT_BUF FILE_BUF

typedef enum inputMode {
    ARGUMENTS, CONSOLE
} inputMode;

typedef struct processInfo {
    size_t convertedFiles;
    size_t deletedFiles;
    double executionTime;
} processInfo;

typedef struct fmtTime {
    size_t hours;
    size_t minutes;
    double seconds;
} fmtTime;

typedef struct arguments {
    char **inPaths;
    char *ffOptions;
    char **inFormats;
    const char *outFormat;

    union OutPath {
        char *customFolder;
        char *customPath;
    } outPath;

    uint8_t options; // Bit fields for the optional arguments
} arguments;

typedef struct Thread {
    char *targetFile;
    size_t outFileID;

#ifdef _WIN32
    HANDLE handle;
#else
    pthread_t handle;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    enum Status {
        RUNNING = 1,
        FINISHED = 2
    } status;
#endif
} Thread;

#define PROGRAM_VERSION      "v1.2.1"
#define CONSOLE_WINDOW_TITLE ("RFFmpeg " PROGRAM_VERSION)

/* ANSI escape chars for colored shell output */
#ifndef NO_ANSI_ESCAPE_CODES
#define COLOR_DEFAULT "\x1b[0m"
#define COLOR_ACCENT  "\x1b[91m"
#define COLOR_INPUT   "\x1b[97m"
#define COLOR_ERROR   "\x1b[91m"
#else
#define COLOR_DEFAULT ""
#define COLOR_ACCENT  ""
#define COLOR_INPUT   ""
#define COLOR_ERROR   ""
#endif

#define LIST_BUF 8
#define LINE_LEN 80

#ifndef _WIN32 // these are defined in win.h for windows
#define FILE_BUF   NAME_MAX
#define ARG_BUF    ARG_MAX
#define PATH_SEP   '/'
#define PREFIX_LEN 0
#endif

#ifdef _WIN32
#define __mt_call_conv unsigned __stdcall
#else
#define __mt_call_conv void *
#endif

#ifdef _WIN32
#define waitForNewLine() {                                          \
    wint_t c;                                                       \
    while ((c = getwchar()) != u'\n' && c != u'\r' && c != WEOF);   \
} (void)0
#else
#define waitForNewLine() {                              \
    int c;                                              \
    while ((c = getchar()) != '\n' && c != EOF);        \
} (void)0
#endif

#define printErr(msg, dsc)                      \
    fprintf(                                    \
            stderr,                             \
            "%s ERROR: %s%s: %s\"%s\"%s\n\n",   \
            COLOR_ERROR,                        \
            COLOR_DEFAULT, msg,                 \
            COLOR_INPUT, dsc,                   \
            COLOR_DEFAULT)

#ifndef NDEBUG
#define dprintf(fmt, ...) fprintf(stderr, "%s:%d:%s(): " fmt,           \
                                  __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define dprintf(...) (void)0
#endif

arguments *allocArguments(void);
fmtTime formatTime(double seconds);
void trimSpaces(char *string);
char *trimUTF8StringTo(const char *str, size_t maxChars);
void readLine(char *dst, size_t dstSize);
bool isDirectory(const char *dir);
bool isZeroMemory(const void *buf, const size_t size);

#endif // H_TYPES
