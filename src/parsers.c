#include "../include/parsers.h"

int _tokenizeArguments(char *string, const char *delimiter, char **destinationAddress[],
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
    _tokenizeArguments(inputPathsString, ":", &arguments->inPaths, LIST_BUFFER);

    free(inputPathsString);

    char *inputFormatsString = NULL;
    size_t inputFormatsSize = 0;

    printf("%s > %sTarget format(s): %s",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&inputFormatsString, &inputFormatsSize, stdin);

    trimWhiteSpaces(inputFormatsString);
    _tokenizeArguments(inputFormatsString, ", ", &arguments->inFormats, LIST_BUFFER);

    free(inputFormatsString);

    size_t ffOptionsSize = 0;

    printf("%s > %sFFmpeg options: %s",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&arguments->ffOptions, &ffOptionsSize, stdin);
    trimWhiteSpaces(arguments->ffOptions);

    size_t outFormatSize = 0;

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

    char **optionsList = xcalloc(LIST_BUFFER, sizeof(char*));

    _tokenizeArguments(optionsString, ", ", &optionsList, LIST_BUFFER);
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
void parseArguments(const int listSize, char *rawArguments[], arguments *parsedArgs) {
    size_t count = listSize == 0 ? SIZE_MAX : listSize;
    
    for (int i = 0; i < count && rawArguments[i] != NULL; i++) {
        /* fmt: -path <path> -in <container> -opts <params> -out <container> */
        if (strcasecmp(rawArguments[i], ARG_INPUTPATHS) == 0) {
            _tokenizeArguments(rawArguments[++i], ":", &parsedArgs->inPaths, LIST_BUFFER);
        }
        
        else if (strcasecmp(rawArguments[i], ARG_INPUTFORMATS) == 0) {
            _tokenizeArguments(rawArguments[++i], ", ", &parsedArgs->inFormats, LIST_BUFFER);
        }
        
        else if (strcasecmp(rawArguments[i], ARG_INPUTPARAMETERS) == 0) {
            parsedArgs->ffOptions = strdup(rawArguments[++i]);
        }
        
        else if (strcasecmp(rawArguments[i], ARG_OUTPUTFORMAT) == 0) {
            parsedArgs->outFormat = strdup(rawArguments[++i]);
        }
        
        else if (strcasecmp(rawArguments[i], OPT_DISPLAYHELP_STRING) == 0) {
            parsedArgs->options |= OPT_DISPLAYHELP;
        }
        
        else if (strcasecmp(rawArguments[i], OPT_CLEANUP_STRING) == 0) {
            parsedArgs->options |= OPT_CLEANUP;
        }
        
        else if (strcasecmp(rawArguments[i], OPT_NORECURSION_STRING) == 0) {
            parsedArgs->options |= OPT_NORECURSION;
        }
        
        else if (strcasecmp(rawArguments[i], OPT_OVERWRITE_STRING) == 0) {
            parsedArgs->options |= OPT_OVERWRITE;
        }
        
        else if (strstr(rawArguments[i], OPT_NEWFOLDER_STRING)) {
            parsedArgs->options |= OPT_NEWFOLDER;

            char *delimiterSection = strstr(rawArguments[i], "=");

            if (delimiterSection != NULL) {
                parsedArgs->options |= OPT_CUSTOMFOLDERNAME;
                
                parsedArgs->customFolderName = strdup(++delimiterSection);
            }
        }
        
        else if (strstr(rawArguments[i], OPT_NEWPATH_STRING)) {
            parsedArgs->options |= OPT_NEWPATH;

            char *delimiterSection = strstr(rawArguments[i], "=");

            if (delimiterSection != NULL) {
                parsedArgs->customPathName = strdup(++delimiterSection);
            }
        }
    }
}

int _tokenizeArguments(char *string, const char *delimiter, char **destinationAddress[],
 size_t numberOfItems) {
    char *parserState = NULL;
    char *token = strtok_r(string, delimiter, &parserState);

    size_t i;

    for (i = 0; token != NULL; i++) {
        /* Reallocate in case the list needs to be longer */
        if (i >= (numberOfItems - 1)) {
            numberOfItems += LIST_BUFFER;

            char **newBlock = realloc(*destinationAddress, numberOfItems * sizeof(char*));

            if (newBlock != NULL)
                *destinationAddress = newBlock;
            else    
                return EXIT_FAILURE;
        }

        trimWhiteSpaces(token);

        (*destinationAddress)[i] = strdup(token);

        token = strtok_r(NULL, delimiter, &parserState);
    }

    (*destinationAddress)[i] = NULL; // NULL ptr to mark the end of the list

    return EXIT_SUCCESS;
}