#ifndef H_TYPES_PRIV
#define H_TYPES_PRIV

#include "libs.h"

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

typedef struct processInformation {
    uint64_t convertedFiles;
    uint64_t deletedFiles;
    double executionTime;
} processInfo_t;

typedef struct formattedTime_t {
    uint64_t hours;
    uint64_t minutes;
    double seconds;
} formattedTime_t;

typedef struct arguments_t {
   char16_t inputPath[PATHBUF];
   char16_t inputFormatString[SHORTBUF];
   char16_t inputParameters[BUFFER];
   char16_t outputFormat[SHORTBUF];
   char16_t customFolderName[PATHBUF];

   bool optionDisplayHelp;
   bool optionMakeNewFolder;
   bool optionCustomFolderName;
   bool optionDeleteOriginalFiles;
   bool optionDisableRecursiveSearch;
   bool optionForceFileOverwrites;
} arguments_t;

#endif