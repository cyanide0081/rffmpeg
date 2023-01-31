#include "../include/libs.h"

int main(int argc, const char *argv[]) {
    char *arguments[MAX_ARGS] = { NULL };

    arguments[ARG_INPATH]        = malloc(PATHBUF);
    arguments[ARG_INFORMAT]      = malloc(BUFFER);
    arguments[ARG_INPARAMETERS]  = malloc(BUFFER);
    arguments[ARG_OUTFORMAT]     = malloc(SHORTBUF);
    arguments[ARG_NEWFOLDERNAME] = malloc(PATHBUF);

    bool options[MAX_OPTS] = { false };

    /* Set system locale and output codepage to UTF-8 for unicode support */
    char originalLocale[PATHBUF] ;
    strcpy_s(originalLocale, PATHBUF, setlocale(LC_ALL, ""));
    setlocale(LC_ALL, ".UTF8");  

    UINT originalConsoleOutputCP = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);

    DWORD originalConsoleMode;
    wchar_t originalConsoleWindowTitle[PATHBUF];
 
    errorCode_t exitCode = ERROR_NONE;
    processInfo_t processInformation = { 0 };
    inputMode_t inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

    enableVirtualTerminalProcessing(&originalConsoleMode);

    if (inputMode == CONSOLE) {
        GetConsoleTitleW(originalConsoleWindowTitle, PATHBUF);
        SetConsoleTitleW(consoleWindowTitle);
    }

    printf_s("%s%s%s\n\n", CHARCOLOR_RED, fullTitle, COLOR_DEFAULT); // print title

    if (inputMode == ARGUMENTS) {
        parseArguments(argc, argv, arguments, options, true, true);
    } else {
        parseArgumentsFromTerminal(arguments, options);
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
        printf_s(" %s(Press any key to exit) %s", CHARCOLOR_WHITE, COLOR_DEFAULT);
        getchar();

        printf_s("\n");
        SetConsoleTitleW(originalConsoleWindowTitle);
    }

    /* Restore our locale/codepage changes before leaving */
    setlocale(LC_ALL, originalLocale);
    SetConsoleOutputCP(originalConsoleOutputCP);
    restoreConsoleMode(originalConsoleMode);

     return (int)exitCode;
}