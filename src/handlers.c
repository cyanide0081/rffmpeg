#include "../include/handlers.h"

int preventFilenameOverwrites(char16_t *pureName, const char16_t *format, const char16_t *path) {
    char16_t fileMask[PATH_BUFFER];
    char16_t newName[PATH_BUFFER];

    swprintf_s(fileMask, PATH_BUFFER, u"%ls\\%ls.%ls", path, pureName, format);

    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW fileData;

    if ((fileHandle = FindFirstFileW(fileMask, &fileData)) != INVALID_HANDLE_VALUE) {
        size_t index = 0;

        do {
            swprintf_s(fileMask, BUFFER, u"%ls\\%ls-%03d.%ls", path, pureName, ++index, format);
        }   while ((fileHandle = FindFirstFileW(fileMask, &fileData)) != INVALID_HANDLE_VALUE);

        swprintf_s(newName, PATH_BUFFER, u"%ls-%03d", pureName, index);
        wcscpy_s(pureName, PATH_BUFFER, newName);
    }

    return NO_ERROR;
}

int handleArgumentErrors(arguments *arguments) {
    /* Set current working directory as input path if none is provided */
    if (*arguments->inPaths[0] == 0) {
        arguments->inPaths[0] = malloc(SHORTBUF * sizeof(char));
        GetCurrentDirectoryW(SHORTBUF, arguments->inPaths[0]);
        arguments->inPathsCount++;
    }

    if (*arguments->inFormats[0] == u'\0') {
        printError(u"no input format (null)");
        return ERROR_NO_INPUT_FORMAT;
    }

    if (*arguments->outFormat == u'\0') {
        printError(u"no output format (null)");
        return ERROR_NO_OUTPUT_FORMAT;
    }

    return NO_ERROR;
}

int createTestProcess(void) {
    STARTUPINFOW ffmpegStartupInfo = { sizeof(ffmpegStartupInfo) };
    PROCESS_INFORMATION ffmpegProcessInfo;
    char16_t ffmpegProcessCall[] = u"ffmpeg -loglevel quiet";

    if (CreateProcessW(NULL, ffmpegProcessCall, NULL, NULL,
     FALSE, 0, NULL, NULL, &ffmpegStartupInfo, &ffmpegProcessInfo)) {
        WaitForSingleObject(ffmpegProcessInfo.hProcess, INFINITE);
        CloseHandle(ffmpegProcessInfo.hProcess);
        CloseHandle(ffmpegProcessInfo.hThread);

        return EXIT_SUCCESS;
    }

    fwprintf_s(stderr, u"%lsERROR:%ls couldn't find FFmpeg (code: %ls%lu%ls)\n\n",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_RED, GetLastError(), CHARCOLOR_WHITE);

    exit(EXIT_FAILURE);
}