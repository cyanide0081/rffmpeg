#include "../include/typefunctions.h"

arguments_t *initializeArguments(void) {
    arguments_t *newArguments = malloc(sizeof(*newArguments));

    newArguments->inputPath              = calloc(PATHBUF,  sizeof(char16_t));
    newArguments->inputFormatString      = calloc(BUFFER,   sizeof(char16_t));
    newArguments->inputParameters        = calloc(BUFFER,   sizeof(char16_t));
    newArguments->outputFormat           = calloc(SHORTBUF, sizeof(char16_t));
    newArguments->customFolderName       = calloc(PATHBUF,  sizeof(char16_t));

    newArguments->optionDisplayHelp            = false;
    newArguments->optionMakeNewFolder          = false;
    newArguments->optionCustomFolderName       = false;
    newArguments->optionDeleteOriginalFiles    = false;
    newArguments->optionDisableRecursiveSearch = false;
    newArguments->optionForceFileOverwrites    = false;

    return newArguments;
}

void freeArguments(arguments_t *arguments) {
    free(arguments->inputPath);
    arguments->inputPath = NULL;

    free(arguments->inputFormatString);
    arguments->inputFormatString = NULL;

    free(arguments->inputParameters);
    arguments->inputParameters = NULL;

    free(arguments->outputFormat);
    arguments->outputFormat = NULL;

    free(arguments->customFolderName);
    arguments->customFolderName = NULL;

    free(arguments);
}