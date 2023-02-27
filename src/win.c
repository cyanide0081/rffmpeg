#ifdef _WIN32

#include "../include/win.h"

ssize_t getline(char **string, size_t *buffer, FILE *stream) {
    #define LARGE_BUF 4096

    wchar_t wideBuf[LARGE_BUF];

    if (fgetws(wideBuf, LARGE_BUF, stream) == NULL)
        return -1;

    int size = WideCharToMultiByte(CP_UTF8, 0, wideBuf, -1, NULL, 0, NULL, NULL);

    char *narrowBuf = xcalloc(size, sizeof(char));

    WideCharToMultiByte(CP_UTF8, 0, wideBuf, -1, narrowBuf, size, NULL, NULL);
    trimSpaces(narrowBuf);

    if (*buffer == 0)
        *string = strdup(narrowBuf);
    else
        memccpy(*string, narrowBuf, '\0', *buffer);

    return strlen(*string);
}

int mkdirWin(const char *dir, int mode) {
    wchar_t dirW[PATH_BUFFER];
    MultiByteToWideChar(CP_UTF8, 0, dir, -1, dirW, PATH_BUFFER);

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
    MultiByteToWideChar(CP_UTF8, 0, dir, -1, dirW, PATH_BUFFER);

    return _wopendir(dirW);
}

// struct dirent *readdirWin(DIR *dir) {

// }

#endif // _WIN32