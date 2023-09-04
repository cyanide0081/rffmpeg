#ifdef _WIN32

#include "../include/win.h"

int clock_gettime(int t, struct timespec *spec) {
    int64_t wintime;
    GetSystemTimeAsFileTime((FILETIME*)&wintime);

    wintime -= 116444736000000000i64;            // 1/jan/1601 to 1/jan/1970
    spec->tv_sec  = wintime / 10000000i64;       //seconds
    spec->tv_nsec = wintime % 10000000i64 * 100; //nano-seconds

    return 0;
}

ssize_t getline(char **string, size_t *buffer, FILE *stream) {
    #define LARGE_BUF 4096
    wchar_t wideBuf[LARGE_BUF];

    if (fgetws(wideBuf, LARGE_BUF, stream) == NULL)
        return -1;

    int size = utf16toutf8(wideBuf, -1, NULL, 0);
    char *narrowBuf = xcalloc(size, sizeof(char));

    utf16toutf8(wideBuf, -1, narrowBuf, size);
    trimSpaces(narrowBuf);

    if (*buffer == 0)
        *string = _strdup(narrowBuf);
    else
        _memccpy(*string, narrowBuf, '\0', *buffer);

    return strlen(*string);
}

int mkdirWin(const char *dir, int mode) {
    wchar_t dirW[PATH_BUFFER];
    utf8toutf16(dir, -1, dirW, PATH_BUFFER);

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
    utf8toutf16(dir, -1, dirW, PATH_BUFFER);

    return _wopendir(dirW);
}

#endif // _WIN32
