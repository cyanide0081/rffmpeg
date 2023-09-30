#ifndef H_WIN
#define H_WIN

/* WinAPI compatibility/abstraction layer */
#include <stdio.h>
#ifdef _WIN32

#include <libs.h>

/* Substitutions for MSVC compiler/libs */
#ifdef _MSC_VER
#include <basetsd.h>

#define strncasecmp         _strnicmp
#define strcasecmp          _stricmp
#define strtok_r            strtok_s
#define ssize_t             SSIZE_T
#define CLOCK_MONOTONIC_RAW 0
#endif /* MSC_VER */

/* max UNICODE_STRING size (subauth.h)  */
#define ARG_BUF  SHRT_MAX
#define FILE_BUF FILENAME_MAX
#define PATH_SEP '\\'
#define PATH_BUF ARG_BUF

#define DIR     _WDIR
#define dirent  _wdirent
#define strdup  _strdup
#define memccpy _memccpy

#define mkdir(a, b) mkdir_WIN(a, b)
#define opendir(d)  opendir_WIN(d)
#define remove(d)   remove_WIN(d)
#define getchar()   (int)getwchar()

#define readdir(d)  _wreaddir(d)
#define closedir(d) _wclosedir(d)

#define UTF8toUTF16(mbs, mbc, wcs, wcc)                 \
    MultiByteToWideChar(CP_UTF8, 0, mbs, mbc, wcs, wcc)
#define UTF16toUTF8(wcs, wcc, mbs, mbc)                             \
    WideCharToMultiByte(CP_UTF8, 0, wcs, wcc, mbs, mbc, NULL, NULL)

#define printWinErrMsg(preamble, err) {                             \
    wchar_t *errMsgW = NULL;                                        \
    int sizeW = FormatMessageW(                                     \
        FORMAT_MESSAGE_ALLOCATE_BUFFER |                            \
        FORMAT_MESSAGE_FROM_SYSTEM |                                \
        FORMAT_MESSAGE_IGNORE_INSERTS,                              \
        NULL,                                                       \
        err,                                                        \
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),                  \
        (LPWSTR)&errMsgW,                                           \
        0,                                                          \
        NULL                                                        \
    );                                                              \
                                                                    \
    int size = UTF16toUTF8(errMsgW, (int)sizeW, NULL, 0);           \
    char *errMsg = xcalloc(size, sizeof(char));                     \
    UTF16toUTF8(errMsgW, sizeW, errMsg, size);                      \
    trimSpaces(errMsg);                                             \
                                                                    \
    printErr(preamble, errMsg);                                     \
    LocalFree(errMsgW);                                             \
    free(errMsg);                                                   \
} (void)0

#define addUnicodePrefixToDir(src, dst) {                   \
    char prefixedDir[PATH_BUF];                             \
    sprintf_s(prefixedDir, PATH_BUF, "\\\\?\\%s", src);     \
    UTF8toUTF16(prefixedDir, -1, dst, PATH_BUF);            \
} (void)0

static char *strndup(const char *str, size_t n) {
    if (strlen(str) <= n) {
        return strdup(str);
    }

    char *r = calloc(n + 1, sizeof(char));

    if (!r) {
        fprintf(stderr, " out of memory: %s", strerror(errno));
        exit(errno);
    }

    memcpy(r, str, n);
    return r;
}

/* Implementation of clock_gettime for win32 */
static int clock_gettime(int clockId, struct timespec *spec) {
    (void)clockId;
    int64_t winTime;
    GetSystemTimeAsFileTime((FILETIME*)&winTime);

    winTime -= 116444736000000000i64;            // (1/jan/1601) to (1/jan/1970)
    spec->tv_sec  = winTime / 10000000i64;       // seconds
    spec->tv_nsec = winTime % 10000000i64 * 100; // nano-seconds

    return 0;
}

/* Overrides mkdir to get around differences between std and ms versions */
static int mkdir_WIN(const char *dir, int mode) {
    (void)mode;

    wchar_t dirW[PATH_BUF];
    WIN32_FIND_DATAW fileData;
    addUnicodePrefixToDir(dir, dirW);

    if (FindFirstFileW(dirW, &fileData) != INVALID_HANDLE_VALUE)
        return EXIT_SUCCESS; // Dir already exists

    if (!CreateDirectoryW(dirW, NULL)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* Overrides opendir() to support long UNICODE directories on Windows */
static DIR *opendir_WIN(const char *dir) {
    char prefixedDir[PATH_BUF];
    sprintf_s(prefixedDir, PATH_BUF, "\\\\?\\%s", dir);

    wchar_t dirW[PATH_BUF];
    addUnicodePrefixToDir(dir, dirW);

    return _wopendir(dirW);
}

static int remove_WIN(const char *dir) {
    wchar_t dirW[PATH_BUF];
    addUnicodePrefixToDir(dir, dirW);

    if (!DeleteFileW(dirW))
        return GetLastError();

    return EXIT_SUCCESS;
}

static int restoreConsoleMode(DWORD originalConsoleMode) {
    HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (SetConsoleMode(handleToStdOut, originalConsoleMode == false))
        return GetLastError();

    return EXIT_SUCCESS;
}

static int enableVirtualTerminalProcessing(PDWORD originalConsoleMode) {
    HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode = 0;

    if (GetConsoleMode(handleToStdOut, &consoleMode) == false)
        return GetLastError();

    *originalConsoleMode = consoleMode;
    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if (SetConsoleMode(handleToStdOut, consoleMode) == false)
        return GetLastError();

    return EXIT_SUCCESS;
}

#endif /* _WIN32 */
#endif /* H_WIN */
