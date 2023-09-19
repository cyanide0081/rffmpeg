#ifndef H_WIN
#define H_WIN

/* WinAPI compatibility/abstraction layer */
#ifdef _WIN32

#include <libs.h>
#include <constants.h>

/* Substitutions for MSVC compiler */
#ifdef _MSC_VER

#include <basetsd.h>

typedef SSIZE_T ssize_t;

#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define strtok_r strtok_s
#define CLOCK_MONOTONIC_RAW 0

#endif

#define DIR _WDIR
#define dirent _wdirent
#define strdup _strdup
#define memccpy _memccpy

#define mkdir(a, b) mkdirWin(a, b)
#define opendir(d) opendirWin(d)
#define readdir(d) _wreaddir(d)
#define closedir(d) _wclosedir(d)

#define UTF8toUTF16(mbs, mbc, wcs, wcc) \
    MultiByteToWideChar(CP_UTF8, 0, mbs, mbc, wcs, wcc)
#define UTF16toUTF8(wcs, wcc, mbs, mbc) \
    WideCharToMultiByte(CP_UTF8, 0, wcs, wcc, mbs, mbc, NULL, NULL)

/* Implementation of clock_gettime for win32 */
int clock_gettime(int, struct timespec *spec);

/* Implementation of POSIX's getline() since it's unavailable in MSVCRT */
ssize_t getline(char **string, size_t *buffer, FILE *stream);

/* Overrides mkdir to get around differences between std and ms versions */
int mkdirWin(const char *dir, int mode);

/* Overrides opendir() to support UNICODE directories on Windows */
DIR *opendirWin(const char *dir);

int clock_gettime(int t, struct timespec *spec) {
    (void)t;

    int64_t wintime;
    GetSystemTimeAsFileTime((FILETIME*)&wintime);

    wintime -= 116444736000000000i64;            // (1/jan/1601) to (1/jan/1970)
    spec->tv_sec  = wintime / 10000000i64;       // seconds
    spec->tv_nsec = wintime % 10000000i64 * 100; // nano-seconds

    return 0;
}

int mkdirWin(const char *dir, int mode) {
    (void)mode;

    wchar_t dirW[PATH_BUFFER];
    UTF8toUTF16(dir, -1, dirW, PATH_BUFFER);

    WIN32_FIND_DATAW fileData;

    if (FindFirstFileW(dirW, &fileData) != INVALID_HANDLE_VALUE)
        return EXIT_SUCCESS; // Dir already exists

    if (!CreateDirectoryW(dirW, NULL)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

DIR *opendirWin(const char *dir) {
    wchar_t dirW[PATH_BUFFER];
    UTF8toUTF16(dir, -1, dirW, PATH_BUFFER);

    return _wopendir(dirW);
}

int restoreConsoleMode(DWORD originalConsoleMode) {
  HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

  if (SetConsoleMode(handleToStdOut, originalConsoleMode == false))
    return GetLastError();

  return EXIT_SUCCESS;
}

int enableVirtualTerminalProcessing(PDWORD originalConsoleMode) {
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

#endif // _WIN32
#endif // H_WIN
