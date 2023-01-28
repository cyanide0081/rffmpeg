#include "libs.h"

/* TODO:    implement custom newfolder naming (--newfolder=foldername) */
/* TODO:    (maybe) implement specific case where overwriting a file of same input fmt as output requires a temp file to be created and renamed afterwards */
/* TODO:    get wchar arguments through windows function instead of wmain() to remove need for the compiler flag */

int main(int argc, const char *argv[]) {
    char *arguments[MAX_ARGS];

    arguments[ARG_INPATH]        = calloc(PATHBUF, sizeof(char));
    arguments[ARG_INFORMAT]      = calloc(BUFFER, sizeof(char));
    arguments[ARG_INPARAMETERS]  = calloc(BUFFER, sizeof(char));
    arguments[ARG_OUTFORMAT]     = calloc(SHORTBUF, sizeof(char));
    arguments[ARG_NEWFOLDERNAME] = calloc(PATHBUF, sizeof(char));

    bool options[MAX_OPTS] = { false };

    setlocale(LC_ALL, "");

    processInfo_t processInformation = { 0 };
    errorCode_t exitCode = ERROR_NONE;
    DWORD originalConsoleMode;
    char originalConsoleWindowTitle[PATHBUF];
    
    // UINT originalConsoleOutputCodePage = GetConsoleOutputCP();
    // SetConsoleOutputCP(CP_UTF8);

    // char commandLineArguments[BUFFER];
    char *cmdArgs = GetCommandLineA();

    for (int i = 0; i < argc; ++i)
        printf("%s\n", argv[i]);
    // size_t bytesWritten;

    // WideCharToMultiByte(CP_UTF8, 0, GetCommandLineW(), wcslen(GetCommandLineW()), commandLineArguments, BUFFER, NULL, NULL);
    
    // wchar_t *wideArgs = GetCommandLineW();

    // wprintf_s(u"%ls\n", wideArgs);
    // printf_s(u8"%s\n", cmdArgs);
   
    // printf(u8"\nワンパンマン");

    inputMode_t inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

    enableVirtualTerminalProcessing(&originalConsoleMode);

    if (inputMode == CONSOLE) {
        GetConsoleTitleA(originalConsoleWindowTitle, PATHBUF);
        SetConsoleTitleA(consoleWindowTitle);
    }

    printf_s(u8"%s%s%s\n\n", CHARCOLOR_RED, fullTitle, COLOR_DEFAULT);

    if (inputMode == ARGUMENTS) {
        parseArguments(argc, argv, arguments, options, true, true);
    } else {
        getInputFromConsole(arguments, options);
    }

    if (options[OPT_DISPLAYHELP] == true && inputMode == ARGUMENTS) {
        displayHelp();
    } else if ((exitCode = handleErrors(arguments)) == ERROR_NONE) {
        clock_t startTime = clock();

        exitCode = searchDirectory(NULL, arguments, options, &processInformation);

        clock_t endTime = clock();

        if (exitCode == ERROR_NONE) {
            processInformation.executionTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;

            displayEndDialog(&processInformation);
        }
    }

    /* Free allocated pointers */
    for (size_t i = 0; i < MAX_ARGS; ++i) {        
        free(arguments[i]);
        arguments[i] = NULL;    
    }

    if (inputMode == CONSOLE) {
        printf_s(u8" %s(Press any key to exit) %s", CHARCOLOR_WHITE, COLOR_DEFAULT);

        getwchar();

        printf_s(u8"\n");
        SetConsoleTitleA(originalConsoleWindowTitle);
    }

    resetConsoleMode(originalConsoleMode);
    // SetConsoleOutputCP(originalConsoleOutputCodePage);

     return (int)exitCode;
}