#if !defined H_WIN && defined _WIN32
#define H_WIN

/* Win32 API compatibility/abstraction layer
 * (uses dirent.h wrapper by Toni Ronkko
 * when compiling with MSVC libraries) */

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

/* ARG_BUF allocates the maximum size of a UNICODE_STRING (subauth.h) and
 * PATH_BUF allocates the maximum size of a qualified (Unicode) WIN32 path */
#define ARG_BUF  USHRT_MAX
#define PATH_BUF SHRT_MAX
#define FILE_BUF FILENAME_MAX
#define PATH_SEP '\\'

/* length of "\\?\" path prefix */
#define PREFIX_LEN 4

#define DIR     _WDIR
#define dirent  _wdirent
#define strdup  _strdup
#define memccpy _memccpy

#define mkdir(a, b) mkdir_WIN(a, b)
#define opendir(d)  opendir_WIN(d)
#define remove(d)   remove_WIN(d)

#define getchar()   getwchar()
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
    char *errMsg = GlobalArenaPush(size * sizeof(char));            \
    UTF16toUTF8(errMsgW, sizeW, errMsg, size);                      \
    trimSpaces(errMsg);                                             \
                                                                    \
    printErr(preamble, errMsg);                                     \
    LocalFree(errMsgW);                                             \
} (void)0

/* prepends "\\?\" to path and replaces '/' with '\' */
#define formatPathToWIN32(src, dst) {                           \
    char prefixedDir[PATH_BUF];                                 \
                                                                \
    if (!strstr(src, "\\\\?\\"))                                \
        sprintf_s(prefixedDir, PATH_BUF, "\\\\?\\%s", src);     \
    else                                                        \
        strncpy(prefixedDir, src, PATH_BUF);                    \
                                                                \
    for (int i = 0; prefixedDir[i]; i++) {                      \
        if (prefixedDir[i] == '/')                              \
            prefixedDir[i] = '\\';                              \
        if (prefixedDir[i] == '\\' && !prefixedDir[i + 1]) {    \
            prefixedDir[i] = '\0';                              \
            break;                                              \
        }                                                       \
    }                                                           \
                                                                \
    UTF8toUTF16(prefixedDir, -1, dst, PATH_BUF);                \
} (void)0

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
    formatPathToWIN32(dir, dirW);

    if (FindFirstFileW(dirW, &fileData) != INVALID_HANDLE_VALUE)
        return EXIT_SUCCESS; // Dir already exists

    if (!CreateDirectoryW(dirW, NULL)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* Overrides opendir() to support long UNICODE directories on Windows */
static DIR *opendir_WIN(const char *dir) {
    wchar_t dirW[PATH_BUF];
    formatPathToWIN32(dir, dirW);

    return _wopendir(dirW);
}

static int remove_WIN(const char *dir) {
    wchar_t dirW[PATH_BUF];
    formatPathToWIN32(dir, dirW);

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

#endif /* H_WIN */
