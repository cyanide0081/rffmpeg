#include "../include/input.h"

int parseArgumentsFromTerminal(char *arguments[], bool *options) {
    printf_s("%s > %sInput path: %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    fgets(arguments[ARG_INPATH], PATHBUF, stdin);
    arguments[ARG_INPATH][strcspn(arguments[ARG_INPATH], "\r\n")] = '\0'; // Remove trailing fgets() newline
    
    printf_s("%s > %sTarget format(s): %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    fgets(arguments[ARG_INFORMAT], BUFFER, stdin);
    arguments[ARG_INFORMAT][strcspn(arguments[ARG_INFORMAT], "\r\n")] = '\0';

    printf_s("%s > %sFFmpeg options: %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    fgets(arguments[ARG_INPARAMETERS], BUFFER, stdin);
    arguments[ARG_INPARAMETERS][strcspn(arguments[ARG_INPARAMETERS], "\r\n")] = '\0';

    printf_s("%s > %sOutput format: %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    fgets(arguments[ARG_OUTFORMAT], SHORTBUF, stdin);
    arguments[ARG_OUTFORMAT][strcspn(arguments[ARG_OUTFORMAT], "\r\n")] = '\0';

    char optionsString[BUFFER];

    printf_s("%s > %sAdditional modes: %s", CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);
    fgets(optionsString, BUFFER, stdin);
    optionsString[strcspn(optionsString, "\r\n")] = '\0';
    printf_s("\n");

    uint16_t numberOfOptions = 0;
     
    char *optionsTokenized[MAX_OPTS];
    char *parserState;
    char *token = strtok_s(optionsString, ", ", &parserState);

    while (token) {
        optionsTokenized[numberOfOptions] = calloc(SHORTBUF, sizeof(char));
        strcpy_s(optionsTokenized[numberOfOptions++], SHORTBUF, token);
        
        token = strtok_s(NULL, " ", &parserState);
    }

    if (numberOfOptions > 0) {
        parseArguments(numberOfOptions, (const char**)optionsTokenized, arguments, options, false, true);
    }

    printf_s(COLOR_DEFAULT);

    for (int i = 0; i < numberOfOptions; ++i)
        free(optionsTokenized[i]);

    return EXIT_SUCCESS;
}

void parseArguments(const int count, const char *rawArguments[], char *parsedArguments[], bool parsedOptions[], bool parseArguments, bool parseOptions) {
    if (parseArguments) {
        /* fmt: -i <path> -f <container> -p <params> -o <container> */
        for (size_t i = 1; i < count; ++i) {
            if (strcmp(rawArguments[i], "-path") == 0) {
                strncpy_s(parsedArguments[ARG_INPATH], PATHBUF - 1, rawArguments[++i], PATHBUF);
            } else if (strcmp(rawArguments[i], "-fmt") == 0) {
                strncpy_s(parsedArguments[ARG_INFORMAT], BUFFER - 1, rawArguments[++i], BUFFER);
            } else if (strcmp(rawArguments[i], "-opts") == 0) {
                strncpy_s(parsedArguments[ARG_INPARAMETERS], BUFFER - 1, rawArguments[++i], BUFFER);
            } else if (strcmp(rawArguments[i], "-ext") == 0) {
                strncpy_s(parsedArguments[ARG_OUTFORMAT], SHORTBUF - 1, rawArguments[++i], SHORTBUF);
            }
        }
    }

    if (parseOptions) {
        /* fmt: --help, --newfolder=foldername, --delete, --norecursion, --overwrite, */
        for (size_t i = 0; i < count; ++i) {
            if (strcmp(rawArguments[i], OPT_DISPLAYHELP_STRING) == 0) {
                parsedOptions[OPT_DISPLAYHELP] = true;
            } else if (strstr(rawArguments[i], OPT_MAKENEWFOLDER_STRING)) {
                parsedOptions[OPT_MAKENEWFOLDER] = true;

                char *argumentBuffer = strdup(rawArguments[i]); // duplicate argument string for analysis

                char *parserState;
                char *delimiter = "=";
                char *token = strtok_s(argumentBuffer, delimiter, &parserState);

                /* If there's an '=' sign, pass the string after it to the foldername argument */
                if ((token = strtok_s(NULL, delimiter, &parserState)) != NULL) {
                    parsedOptions[OPT_CUSTOMFOLDERNAME] = true;
                    
                    strcpy_s(parsedArguments[ARG_NEWFOLDERNAME], PATHBUF, token);
                }

                free(argumentBuffer);
            } else if (strcmp(rawArguments[i], OPT_DELETEOLDFILES_STRING) == 0) {
                parsedOptions[OPT_DELETEOLDFILES] = true;
            } else if (strcmp(rawArguments[i], OPT_DISABLERECURSION_STRING) == 0) {
                parsedOptions[OPT_DISABLERECURSION] = true;
            } else if (strcmp(rawArguments[i], OPT_FORCEOVERWRITE_STRING) == 0) {
                parsedOptions[OPT_FORCEOVERWRITE] = true;
            }
        }
    }
}