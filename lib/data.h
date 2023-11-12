#ifndef H_TYPES
#define H_TYPES

#include <libs.h>
#include <arena.h>

#ifdef _WIN32
#include <win.h>
#endif

#define FMT_BUF FILE_BUF

typedef enum InputMode {
    ARGUMENTS,
    CONSOLE
} InputMode;

typedef struct ProcessInfo {
    size_t totalFiles;
    size_t convertedFiles;
    size_t deletedFiles;
    double executionTime;
} ProcessInfo;

typedef struct FmtTime {
    size_t hours;
    size_t minutes;
    double seconds;
} FmtTime;

typedef struct Arguments {
    char **inPaths;
    char *ffOptions;
    char **inFormats;
    const char *outFormat;
    size_t numberOfThreads;

    union OutPath {
        char *customFolder;
        char *customPath;
    } outPath;

    uint16_t options; // fields for the optional arguments
} Arguments;

typedef struct Thread {
    void *callArg;
    char *targetFile;
    size_t outFileID;

#ifdef _WIN32
    HANDLE handle;
#else
    pthread_t handle;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
#endif
} Thread;

#define VERSION_NUMBER       "1.3.4"
#define VERSION_NAME         "(线程)"
#define VERSION_DESC         "RFFMPEG v" VERSION_NUMBER " " VERSION_NAME
#define CONSOLE_WINDOW_TITLE ("RFFmpeg v" VERSION_NUMBER)

#if defined _WIN32
#define CUR_OS "windows"
#elif defined __linux__
#define CUR_OS "linux"
#elif defined __APPLE__
#define CUR_OS "macos"
#elif defined __FreeBSD__
#define CUR_OS "freebsd"
#else
#define CUR_OS "[unrecognized_os]"
#endif

#if defined __x86_64__ || defined __amd64__
#define CUR_ARCH "x86_64"
#elif defined __i386__ || defined __i486__ || defined __i586__ || defined __i686__
#define CUR_ARCH "x86"
#elif defined __aarch64__
#define CUR_ARCH "arm64"
#elif defined __arm__
#define CUR_ARCH "arm"
#else
#define CUR_ARCH "[unrecognized_arch]"
#endif

#if defined NDEBUG
#define CUR_MODE "release"
#elif defined INSTRUMENTATION
#define CUR_MODE "profiling"
#else
#define CUR_MODE "debug"
#endif

#define VERSION_ATTRIBUTES \
    "rffmpeg-" CUR_MODE " version " VERSION_NUMBER " for " CUR_ARCH "-" CUR_OS

/* ANSI escape chars for colored shell output */
#define LINE_ERASE    "\x1b[2K"
#define LINE_MOVE_UP  "\x1b[A"
#define CARRIAGE_RET  "\r"

#ifndef NO_ANSI_COLOR_CODES
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
    while ((c = getwchar()) != L'\n' && c != L'\r' && c != WEOF);   \
} (void)0
#else
#define waitForNewLine() {                              \
    int c;                                              \
    while ((c = getchar()) != '\n' && c != EOF);        \
} (void)0
#endif

#define printErr(msg, dsc)                      \
    fprintf(                                    \
        stderr,                                 \
        "%s ERROR: %s%s: %s%s%s\n\n",           \
        COLOR_ERROR,                            \
        COLOR_DEFAULT, msg,                     \
        COLOR_INPUT, dsc,                       \
        COLOR_DEFAULT                           \
    )                                           \

#ifndef NDEBUG
#define dprintf(fmt, ...)                                                     \
    fprintf(                                                                  \
        stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__ \
    )
#else
#define dprintf(...) (void)0
#endif

Arguments *ArgumentsAlloc(void);
FmtTime formatTime(double seconds);
void trimSpaces(char *string);
size_t stringConcat(char *dst, const char *src);
char *trimUTF8StringTo(const char *str, size_t maxChars);
void readLine(char *dst, size_t dstSize);
bool isDirectory(const char *dir);
bool isZeroMemory(const void *buf, const size_t bytes);
size_t getNumberOfOnlineThreads(void);
char *getAbsolutePath(const char *dir);
char *getCurrentWorkingDirectory(void);
void printVersionPage(const char *arg);

#ifndef _WIN32
void threadAttrInit(pthread_attr_t *attr);
void threadAttrDestroy(pthread_attr_t *attr);
#endif

#endif // H_TYPES
