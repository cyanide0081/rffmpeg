#ifndef H_TYPES_PRIV
#define H_TYPES_PRIV

#include "libs.h"

typedef struct processInformation {
    uint64_t convertedFiles;
    uint64_t deletedFiles;
    double executionTime;
} processInfo_t;

typedef enum inputMode {
    ARGUMENTS, CONSOLE
} inputMode_t;

typedef enum errorCode {
    ERROR_NONE,
    ERROR_FAILED_SETCONSOLEMODE,
    ERROR_FAILED_TO_OPEN_DIRECTORY,
    ERROR_NO_INPUT_FORMAT,
    ERROR_NO_OUTPUT_FORMAT
} errorCode_t;

typedef struct arguments_t {
   char16_t *inputPath;
   char16_t *inputFormatString;
   char16_t *inputParameters;
   char16_t *outputFormat;
   char16_t *customFolderName;

   bool optionDisplayHelp;
   bool optionMakeNewFolder;
   bool optionCustomFolderName;
   bool optionDeleteOriginalFiles;
   bool optionDisableRecursiveSearch;
   bool optionForceFileOverwrites;
} arguments_t;

#endif