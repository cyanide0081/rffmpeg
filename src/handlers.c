#include "../include/handlers.h"

int preventFilenameOverwrites(char16_t *pureFilename, const char16_t *outputFormat, const char16_t *path) {
    char16_t fileMask[PATH_BUFFER];
    char16_t fileNameNew[PATH_BUFFER];

    swprintf_s(fileMask, PATH_BUFFER, u"%ls\\%ls.%ls", path, pureFilename, outputFormat);

    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW fileData;

    if ((fileHandle = FindFirstFileW(fileMask, &fileData)) != INVALID_HANDLE_VALUE) {
        size_t index = 0;

        do {
            swprintf_s(fileMask, BUFFER, u"%ls\\%ls-%03d.%ls", path, pureFilename, ++index, outputFormat);
        }   while ((fileHandle = FindFirstFileW(fileMask, &fileData)) != INVALID_HANDLE_VALUE);

        swprintf_s(fileNameNew, PATH_BUFFER, u"%ls-%03d", pureFilename, index);
        wcscpy_s(pureFilename, PATH_BUFFER, fileNameNew);
    }

    return NO_ERROR;
}

int handleErrors(arguments *arguments) {
    /* Set current working directory as input path if none is provided */
    if (*arguments->inputPaths[0] == 0) {
        arguments->inputPaths[0] = malloc(SHORTBUF * sizeof(char));
        GetCurrentDirectoryW(SHORTBUF, arguments->inputPaths[0]);
        arguments->inputPathsCount++;
    }

    if (*arguments->inputFormats[0] == u'\0') {
        printError(u"no input format (null)");
        return ERROR_NO_INPUT_FORMAT;
    }

    if (*arguments->outputFormat == u'\0') {
        printError(u"no output format (null)");
        return ERROR_NO_OUTPUT_FORMAT;
    }

    return NO_ERROR;
}