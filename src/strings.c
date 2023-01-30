#include "../include/strings.h"

errno_t prependDotToString(char *string, size_t sizeInBytes) {
    size_t finalSize = 0;
    size_t reasonableBuffer = sizeInBytes + 1;

    errno_t errorCode = 0;

    char *finalString = malloc(reasonableBuffer);

    if (finalString == NULL)
        return ENOMEM;

    errorCode = strcpy_s(finalString, reasonableBuffer, ".");
    errorCode = strcat_s(finalString, reasonableBuffer, string);
    errorCode = strcpy_s(string, sizeInBytes, finalString);

    free(finalString);
    finalString = NULL;

    return errorCode;
}