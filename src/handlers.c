#include "../include/handlers.h"

int preventFilenameOverwrites(char16_t *pureFilename, const char16_t *outputFormat, const char16_t *path) {
    char16_t fileMask[PATHBUF];
    char16_t fileNameNew[PATHBUF];

    swprintf_s(fileMask, PATHBUF, u"%ls\\%ls.%ls", path, pureFilename, outputFormat);

    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW fileData;

    if ((fileHandle = FindFirstFileW(fileMask, &fileData)) != INVALID_HANDLE_VALUE) {
        size_t index = 0;

        do {
            swprintf_s(fileMask, BUFFER, u"%ls\\%ls-%03d.%ls", path, pureFilename, ++index, outputFormat);
        }   while ((fileHandle = FindFirstFileW(fileMask, &fileData)) != INVALID_HANDLE_VALUE);

        swprintf_s(fileNameNew, PATHBUF, u"%ls-%03d", pureFilename, index);
        wcscpy_s(pureFilename, PATHBUF, fileNameNew);
    }

    return NO_ERROR;
}

errorCode_t handleErrors(arguments_t *arguments) {
    /* Set current working directory as input path if none is provided */
    if (*arguments->inputPath == 0) {
        GetCurrentDirectoryW(SHORTBUF, arguments->inputPath);
    }

    if (*arguments->inputFormatString == u'\0') {
        printError(u"no input format (null)");
        return ERROR_NO_INPUT_FORMAT;
    }

    if (*arguments->outputFormat == u'\0') {
        printError(u"no output format (null)");
        return ERROR_NO_OUTPUT_FORMAT;
    }

    return ERROR_NONE;
}