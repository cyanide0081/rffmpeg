#include "../include/parsers.h"

static int _tokenizeArguments(char *string, const char *delimiter, char *destination[],
 size_t numberOfItems);
 
/* Parses the argument strings from direct console input in case no argument is given */
int parseConsoleInput(arguments *arguments) {
    int errorCode = EXIT_SUCCESS;
    size_t currentIndex = 0;
    
    char *inputPathsString = NULL;
    size_t inputPathsSize = 0;

    printf("%s > %sInput path(s): %s",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&inputPathsString, &inputPathsSize, stdin);

    trimWhiteSpaces(inputPathsString);
    _tokenizeArguments(inputPathsString, ":", arguments->inPaths, SHORTBUF);

    free(inputPathsString);

    char *inputFormatsString = NULL;
    size_t inputFormatsSize = 0;

    printf("%s > %sTarget format(s): %s",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&inputFormatsString, &inputFormatsSize, stdin);

    trimWhiteSpaces(inputFormatsString);
    _tokenizeArguments(inputFormatsString, ", ", arguments->inFormats, SHORTBUF);

    free(inputFormatsString);

    size_t ffOptionsSize = BUFFER;

    printf("%s > %sFFmpeg options: %s",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&arguments->ffOptions, &ffOptionsSize, stdin);
    trimWhiteSpaces(arguments->ffOptions);

    size_t outFormatSize = SHORTBUF;

    printf("%s > %sOutput extension: %s",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&arguments->outFormat, &outFormatSize, stdin);
    trimWhiteSpaces(arguments->outFormat);

    char *optionsString = NULL;
    size_t optionsStringSize = 0;

    printf("%s > %sAdditional flags: %s",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&optionsString, &optionsStringSize, stdin);
    trimWhiteSpaces(optionsString);

    char *optionsList[SHORTBUF];

    _tokenizeArguments(optionsString, ", ", optionsList, SHORTBUF);
    parseArguments(0, optionsList, arguments);

    for (int i = 0; optionsList[i]; i++) {
        free(optionsList[i]);
        optionsList[i] = NULL;
    }

    printf("\n");
    printf(COLOR_DEFAULT);

    return errorCode;
}

/* Parses an array of strings to format an (arguments*) accordingly */
int parseArguments(const int listSize, char *rawArguments[], arguments *parsedArguments) {
    int errorCode = EXIT_SUCCESS;

    size_t count = listSize == 0 ? SIZE_MAX : listSize;
    
    for (int i = 0; i < count && rawArguments[i]; i++) {
        /* fmt: -i <path> -f <container> -p <params> -o <container> */
        if (strcasecmp(rawArguments[i], ARG_INPUTPATHS) == 0) {
            _tokenizeArguments(rawArguments[++i], ":", parsedArguments->inPaths, SHORTBUF);
        } else if (strcasecmp(rawArguments[i], ARG_INPUTFORMATS) == 0) {
            _tokenizeArguments(rawArguments[++i], ", ", parsedArguments->inFormats, SHORTBUF);
        } else if (strcasecmp(rawArguments[i], ARG_INPUTPARAMETERS) == 0) {
            memccpy(parsedArguments->ffOptions, rawArguments[++i], '\0', BUFFER);
            /* TODO: test this one (also allocate for the arguments here) ^^^^^^ */
        } else if (strcasecmp(rawArguments[i], ARG_OUTPUTFORMAT) == 0) {
            memccpy(parsedArguments->outFormat, rawArguments[++i], '\0', SHORTBUF);
        } else if (strcasecmp(rawArguments[i], OPT_DISPLAYHELP_STRING) == 0) {
            parsedArguments->options |= OPT_DISPLAYHELP;
        } else if (strcasecmp(rawArguments[i], OPT_CLEANUP_STRING) == 0) {
            parsedArguments->options |= OPT_CLEANUP;
        } else if (strcasecmp(rawArguments[i], OPT_NORECURSION_STRING) == 0) {
            parsedArguments->options |= OPT_NORECURSION;
        } else if (strcasecmp(rawArguments[i], OPT_OVERWRITE_STRING) == 0) {
            parsedArguments->options |= OPT_OVERWRITE;
        } else if (strstr(rawArguments[i], OPT_NEWFOLDER_STRING)) {
            parsedArguments->options |= OPT_NEWFOLDER;

            char *delimiterSection = NULL;

            if ((delimiterSection = strstr(rawArguments[i], "=")) != NULL) {
                parsedArguments->options |= OPT_CUSTOMFOLDERNAME;
                
                memccpy(parsedArguments->customFolderName, ++delimiterSection, '\0', FILE_BUFFER);
            }
        }
    }

    return errorCode;
}

static int _tokenizeArguments(char *string, const char *delimiter, char *destination[],
 size_t numberOfItems) {
    char *parserState = NULL;
    char *token = strtok_r(string, delimiter, &parserState);

    size_t index = 0;

    while (token != NULL) {
        /* Reallocate in case the list needs to be longer */
        if (index > (numberOfItems - 2)) {
            numberOfItems += numberOfItems / 2;
            destination = realloc(destination, numberOfItems * sizeof(char*));
        }

        destination[index] = xcalloc((strlen(token) + 1), sizeof(char));
    
        trimWhiteSpaces(token);
        memccpy(destination[(index)++], token, '\0', strlen(token) + 1);
        token = strtok_r(NULL, delimiter, &parserState);
    }

    destination[index] = NULL; // NULL ptr to mark the end of the list

    return EXIT_SUCCESS;
}