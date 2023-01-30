#include "../include/handlers.h"

int preventFilenameOverwrites(char *pureFilename, const char *outputFormat, const char *path) {
    char fileMask[PATHBUF];
    wchar_t fileMaskWide[PATHBUF];
    char fileNameNew[PATHBUF];

    sprintf_s(fileMask, PATHBUF, "%s\\%s.%s", path, pureFilename, outputFormat);
    MultiByteToWideChar(CP_UTF8, 0, fileMask, -1, fileMaskWide, PATHBUF);

    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW fileData;

    if ((fileHandle = FindFirstFileW(fileMaskWide, &fileData)) != INVALID_HANDLE_VALUE) {
        size_t index = 0;

        do {
            sprintf_s(fileMask, BUFFER, "%s\\%s-%03d.%s", path, pureFilename, ++index, outputFormat);
            MultiByteToWideChar(CP_UTF8, 0, fileMask, -1, fileMaskWide, PATHBUF);
        }   while ((fileHandle = FindFirstFileW(fileMaskWide, &fileData)) != INVALID_HANDLE_VALUE);

        sprintf_s(fileNameNew, PATHBUF, "%s-%03d", pureFilename, index);
        strcpy_s(pureFilename, PATHBUF, fileNameNew);
    }

    return EXIT_SUCCESS;
}

errorCode_t handleErrors(char *arguments[]) {
    /* Set current working directory as input path if none is provided */
    if (*arguments[ARG_INPATH] == 0) {
        wchar_t currentDirectory[PATHBUF];
        GetCurrentDirectoryW(PATHBUF, currentDirectory);
        WideCharToMultiByte(CP_UTF8, 0, currentDirectory, -1, arguments[ARG_INPATH], PATHBUF, NULL, NULL);
    }

    if (*arguments[ARG_INFORMAT] == '\0') {
        printError("no input format (null)");
        return ERROR_NO_INPUT_FORMAT;
    }

    if (*arguments[ARG_OUTFORMAT] == '\0') {
        printError("no output format (null)");
        return ERROR_NO_OUTPUT_FORMAT;
    }

    return ERROR_NONE;
}