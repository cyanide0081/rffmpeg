#ifdef _WIN32

#include "../lib/win.h"

int clock_gettime(int t, struct timespec *spec) {
    (void)t;
    
    int64_t wintime;
    GetSystemTimeAsFileTime((FILETIME*)&wintime);

    wintime -= 116444736000000000i64;            // 1/jan/1601 to 1/jan/1970
    spec->tv_sec  = wintime / 10000000i64;       //seconds
    spec->tv_nsec = wintime % 10000000i64 * 100; //nano-seconds

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

#endif /* _WIN32 */
