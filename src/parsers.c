#include "../include/parsers.h"

static int _tokenizeArguments(char16_t *string, const char16_t *delimiter, char16_t *destination[],
 size_t *destinationSize);
 
/* Parses the argument strings from direct console input in case no argument is given */
int parseConsoleInput(arguments *arguments) {
    int errorCode = NO_ERROR;
    size_t currentIndex = 0;
    DWORD charactersRead = 0;
    HANDLE consoleInput = GetStdHandle(STD_INPUT_HANDLE);
    
    char16_t inputPathsString[SHORTBUF * PATH_BUFFER];

    wprintf_s(u"%ls > %lsInput path(s): %ls",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    ReadConsoleW(consoleInput, inputPathsString, PATH_BUFFER, &charactersRead, NULL);
    trimWhiteSpaces(inputPathsString);

    _tokenizeArguments(inputPathsString, u"*", arguments->inPaths, &(arguments->inPathsCount));

    char16_t inputFormatsString[SHORTBUF * 8];

    wprintf_s(u"%ls > %lsTarget format(s): %ls",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    ReadConsoleW(consoleInput, inputFormatsString, SHORTBUF, &charactersRead, NULL);
    trimWhiteSpaces(inputFormatsString);

    _tokenizeArguments(inputFormatsString, u", ", arguments->inFormats,
     &(arguments->inFormatsCount));

    wprintf_s(u"%ls > %lsFFmpeg options: %ls",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    ReadConsoleW(consoleInput, arguments->ffOptions, BUFFER, &charactersRead, NULL);
    trimWhiteSpaces(arguments->ffOptions);
    
    wprintf_s(u"%ls > %lsOutput extension: %ls",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    ReadConsoleW(consoleInput, arguments->outFormat, SHORTBUF, &charactersRead, NULL);
    trimWhiteSpaces(arguments->outFormat);

    char16_t optionsString[BUFFER];

    wprintf_s(u"%ls > %lsAdditional flags: %ls",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    ReadConsoleW(consoleInput, optionsString, BUFFER, &charactersRead, NULL);
    trimWhiteSpaces(optionsString);

    char16_t *optionsList[SHORTBUF] = { NULL };
    size_t optionsCount = 0;

    _tokenizeArguments(optionsString, u", ", optionsList, &optionsCount);
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
    if (parsedArguments->inPaths[0] == NULL)
        parsedArguments->inPaths[0] = u"";
    if (parsedArguments->inFormats[0] == NULL)
        parsedArguments->inFormats[0] = u"";

    for (int i = 0; i < count; i++) {
        /* fmt: -i <path> -f <container> -p <params> -o <container> */
        if (wcsicmp(rawArguments[i], ARG_INPUTPATHS) == 0) {
            _tokenizeArguments(rawArguments[++i], u"*", parsedArguments->inPaths,
             &(parsedArguments->inPathsCount));
        } else if (wcsicmp(rawArguments[i], ARG_INPUTFORMATS) == 0) {
            _tokenizeArguments(rawArguments[++i], u", ",
             parsedArguments->inFormats, &(parsedArguments->inFormatsCount));
        } else if (wcsicmp(rawArguments[i], ARG_INPUTPARAMETERS) == 0) {
            wcsncpy_s(parsedArguments->ffOptions, BUFFER, rawArguments[++i], BUFFER);
        } else if (wcsicmp(rawArguments[i], ARG_OUTPUTFORMAT) == 0) {
            wcsncpy_s(parsedArguments->outFormat, SHORTBUF, rawArguments[++i], SHORTBUF);
        } else if (wcsicmp(rawArguments[i], OPT_DISPLAYHELP_STRING) == 0) {
            parsedArguments->options |= OPT_DISPLAYHELP;
        } else if (wcsicmp(rawArguments[i], OPT_CLEANUP_STRING) == 0) {
            parsedArguments->options |= OPT_CLEANUP;
        } else if (wcsicmp(rawArguments[i], OPT_NORECURSION_STRING) == 0) {
            parsedArguments->options |= OPT_NORECURSION;
        } else if (wcsicmp(rawArguments[i], OPT_OVERWRITE_STRING) == 0) {
            parsedArguments->options |= OPT_OVERWRITE;
        } else if (wcsstr(rawArguments[i], OPT_NEWFOLDER_STRING)) {
            parsedArguments->options |= OPT_NEWFOLDER;

            char16_t *delimiterSection = NULL;

            if ((delimiterSection = wcsstr(rawArguments[i], u"=")) != NULL) {
                parsedArguments->options |= OPT_CUSTOMFOLDERNAME;
                
                wcscpy_s(parsedArguments->customFolderName, PATH_BUFFER, ++delimiterSection);
            }
        }
    }

    return errorCode;
}

static int _tokenizeArguments(char16_t *string, const char16_t *delimiter, char16_t *destination[],
 size_t *destinationSize) {
    char16_t *parserState = NULL;
    char16_t *token = wcstok_s(string, delimiter, &parserState);

    *destinationSize = 0;

    while (token != NULL) {
        destination[*destinationSize] = xcalloc((wcslen(token) + 1), sizeof(char16_t));
    
        trimWhiteSpaces(token);
        wcscpy_s(destination[(*destinationSize)++], PATH_BUFFER, token);
        token = wcstok_s(NULL, u"*", &parserState);
    }

    return NO_ERROR;
}