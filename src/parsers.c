#include "../include/parsers.h"

static int _tokenizeArguments(char16_t *string, const char16_t *delimiter, char16_t *destinationList[], size_t *destinationItemsCount);

/* Parses the argument strings from direct console input in case no argument is given */
int parseConsoleInput(arguments *arguments) {
    int errorCode = NO_ERROR;
    size_t currentIndex = 0;
    DWORD charactersRead = 0;
    HANDLE consoleInput = GetStdHandle(STD_INPUT_HANDLE);
    
    char16_t inputPathsString[SHORTBUF * PATH_BUFFER];

    wprintf_s(u"%ls > %lsInput path(s): %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, inputPathsString, PATH_BUFFER, &charactersRead, NULL);
    trimWhiteSpaces(inputPathsString);

    if ((errorCode = _tokenizeArguments(inputPathsString, u"*", arguments->inputPaths, &(arguments->inputPathsCount))) != NO_ERROR)
        return errorCode;

    char16_t inputFormatsString[SHORTBUF * 8];

    wprintf_s(u"%ls > %lsTarget format(s): %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, inputFormatsString, SHORTBUF, &charactersRead, NULL);
    trimWhiteSpaces(inputFormatsString);

    if ((errorCode = _tokenizeArguments(inputFormatsString, u", ", arguments->inputFormats, &(arguments->inputFormatsCount))) != NO_ERROR)
        return errorCode;

    wprintf_s(u"%ls > %lsFFmpeg options: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, arguments->ffmpegOptions, BUFFER, &charactersRead, NULL);
    trimWhiteSpaces(arguments->ffmpegOptions);
    
    wprintf_s(u"%ls > %lsOutput extension: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, arguments->outputFormat, SHORTBUF, &charactersRead, NULL);
    trimWhiteSpaces(arguments->outputFormat);

    char16_t optionsString[BUFFER];

    wprintf_s(u"%ls > %lsAdditional flags: %ls", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    ReadConsoleW(consoleInput, optionsString, BUFFER, &charactersRead, NULL);
    trimWhiteSpaces(optionsString);

    char16_t *optionsList[SHORTBUF] = { NULL };
    size_t optionsCount = 0;

    if ((errorCode = _tokenizeArguments(optionsString, u", ", optionsList, &optionsCount)) != NO_ERROR)
        return errorCode;

    parseArguments(optionsCount, optionsList, arguments);

    for (int i = 0; i < optionsCount; i++) {
        free(optionsList[i]);
        optionsList[i] = NULL;
    }

    wprintf_s(u"\n");
    wprintf_s(COLOR_DEFAULT);

    return errorCode;

}
/* Parses an array of strings to format an (arguments*) accordingly */
int parseArguments(const int count, char16_t *rawArguments[], arguments *parsedArguments) {
    int errorCode = NO_ERROR;

    /* Point first input path and format to empty literals */
    if (parsedArguments->inputPaths[0] == NULL)
        parsedArguments->inputPaths[0] = u"";
    if (parsedArguments->inputFormats[0] == NULL)
        parsedArguments->inputFormats[0] = u"";

    for (int i = 0; i < count; i++) {
        /* fmt: -i <path> -f <container> -p <params> -o <container> */
        if (wcsicmp(rawArguments[i], ARG_INPUTPATHS) == 0) {
            if ((errorCode = _tokenizeArguments(rawArguments[++i], u"*", parsedArguments->inputPaths, &(parsedArguments->inputPathsCount))) != NO_ERROR)
                return errorCode;
        } else if (wcsicmp(rawArguments[i], ARG_INPUTFORMATS) == 0) {
            if ((errorCode = _tokenizeArguments(rawArguments[++i], u", ", parsedArguments->inputFormats, &(parsedArguments->inputFormatsCount))) != NO_ERROR)
                return errorCode;
        } else if (wcsicmp(rawArguments[i], ARG_INPUTPARAMETERS) == 0) {
            wcsncpy_s(parsedArguments->ffmpegOptions, BUFFER, rawArguments[++i], BUFFER);
        } else if (wcsicmp(rawArguments[i], ARG_OUTPUTFORMAT) == 0) {
            wcsncpy_s(parsedArguments->outputFormat, SHORTBUF, rawArguments[++i], SHORTBUF);
        } else if (wcsicmp(rawArguments[i], OPT_DISPLAYHELP_STRING) == 0) {
            parsedArguments->options |= OPT_DISPLAYHELP;
        } else if (wcsicmp(rawArguments[i], OPT_DELETEOLDFILES_STRING) == 0) {
            parsedArguments->options |= OPT_DELETEORIGINALFILES;
        } else if (wcsicmp(rawArguments[i], OPT_DISABLERECURSION_STRING) == 0) {
            parsedArguments->options |= OPT_DISABLERECURSION;
        } else if (wcsicmp(rawArguments[i], OPT_FORCEOVERWRITE_STRING) == 0) {
            parsedArguments->options |= OPT_FORCEFILEOVERWRITES;
        } else if (wcsstr(rawArguments[i], OPT_MAKENEWFOLDER_STRING)) {
            parsedArguments->options |= OPT_MAKENEWFOLDER;

            char16_t *delimiterSection = NULL;

            if ((delimiterSection = wcsstr(rawArguments[i], u"=")) != NULL) {
                parsedArguments->options |= OPT_CUSTOMFOLDERNAME;
                
                wcscpy_s(parsedArguments->customFolderName, PATH_BUFFER, ++delimiterSection);
            }
        }
    }

    return errorCode;
}

static int _tokenizeArguments(char16_t *string, const char16_t *delimiter, char16_t *destinationList[], size_t *destinationItemsCount) {
    char16_t *parserState = NULL;
    char16_t *token = wcstok_s(string, delimiter, &parserState);

    *destinationItemsCount = 0;

    while (token != NULL) {
        destinationList[*destinationItemsCount] = xcalloc((wcslen(token) + 1), sizeof(char16_t));
    
        trimWhiteSpaces(token);
        wcscpy_s(destinationList[(*destinationItemsCount)++], PATH_BUFFER, token);
        token = wcstok_s(NULL, u"*", &parserState);
    }

    return NO_ERROR;
}