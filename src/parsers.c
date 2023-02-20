#include "../include/parsers.h"

static char **_tokenizeArguments(char *string, const char *delimiter);
 
/* Parses the argument strings from direct console input in case no argument is given */
void parseConsoleInput(arguments *args) {
    char *inputPathsString = NULL;
    size_t inputPathsSize = 0;

    printf("%s > %sInput path(s): %s",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&inputPathsString, &inputPathsSize, stdin);
    trimWhiteSpaces(inputPathsString);

    args->inPaths = _tokenizeArguments(inputPathsString, ":");

    free(inputPathsString);

    char *inputFormatsString = NULL;
    size_t inputFormatsSize = 0;

    printf("%s > %sTarget format(s): %s",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);


    getline(&inputFormatsString, &inputFormatsSize, stdin);
    trimWhiteSpaces(inputFormatsString);

    args->inFormats = _tokenizeArguments(inputFormatsString, ", ");

    free(inputFormatsString);

    size_t ffOptionsSize = 0;

    printf("%s > %sFFmpeg options: %s",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&args->ffOptions, &ffOptionsSize, stdin);
    trimWhiteSpaces(args->ffOptions);

    size_t outFormatSize = 0;

    printf("%s > %sOutput extension: %s",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&args->outFormat, &outFormatSize, stdin);
    trimWhiteSpaces(args->outFormat);

    char *optionsString = NULL;
    size_t optionsStringSize = 0;

    printf("%s > %sAdditional flags: %s",
     CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&optionsString, &optionsStringSize, stdin);
    trimWhiteSpaces(optionsString);

    printf("\n");
    printf(COLOR_DEFAULT);

    char **optionsList = _tokenizeArguments(optionsString, ", ");
    parseArguments(0, optionsList, args);

    for (int i = 0; optionsList[i] != NULL; i++)
        free(optionsList[i]);

    free(optionsList);
}

/* Parses an array of strings to format an (arguments*) accordingly */
void parseArguments(const int listSize, char *rawArguments[], arguments *parsedArgs) {
    size_t count = listSize == 0 ? SIZE_MAX : listSize;
    
    for (int i = 0; i < count && rawArguments[i] != NULL; i++) {
        /* fmt: -path <path> -in <container> -opts <params> -out <container> */
        if (strcasecmp(rawArguments[i], ARG_INPUTPATHS) == 0) {
            parsedArgs->inPaths = _tokenizeArguments(rawArguments[++i], ":");
        }
        
        else if (strcasecmp(rawArguments[i], ARG_INPUTFORMATS) == 0) {
            parsedArgs->inFormats = _tokenizeArguments(rawArguments[++i], ", ");
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

static char **_tokenizeArguments(char *string, const char *delimiter) {
    char *parserState = NULL;
    char *token = strtok_r(string, delimiter, &parserState);

    size_t items = LIST_BUFFER;

    char **list = xcalloc(items, sizeof(char*));

    size_t i;

    for (i = 0; token != NULL; i++) {
        /* Reallocate in case the list needs to be longer */
        if (i >= (items - 1)) {
            items += LIST_BUFFER;

            char **newBlock = realloc(list, items * sizeof(char*));

            if (newBlock != NULL)
                list = newBlock;
            else    
                return NULL;
        }

        trimWhiteSpaces(token);

        (list)[i] = strdup(token);

        token = strtok_r(NULL, delimiter, &parserState);
    }

    (list)[i] = NULL; // NULL ptr to mark the end of the list

    return list;
}