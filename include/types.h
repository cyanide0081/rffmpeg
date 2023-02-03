#ifndef H_TYPES_PRIV
#define H_TYPES_PRIV

#include "libs.h"

typedef enum inputMode {
    ARGUMENTS, CONSOLE
} inputMode;

typedef struct processInformation {
    uint64_t convertedFiles;
    uint64_t deletedFiles;
    double executionTime;
} processInfo;

typedef struct formattedTime {
    uint64_t hours;
    uint64_t minutes;
    double seconds;
} formattedTime;

typedef struct parsedArguments {
   char16_t inputPaths[PATHBUF];
   char16_t inputFormats[SHORTBUF];
   char16_t inputParameters[BUFFER];
   char16_t outputFormat[SHORTBUF];
   char16_t customFolderName[PATHBUF];

   bool optionDisplayHelp;
   bool optionMakeNewFolder;
   bool optionCustomFolderName;
   bool optionDeleteOriginalFiles;
   bool optionDisableRecursiveSearch;
   bool optionForceFileOverwrites;
} arguments;

#endif