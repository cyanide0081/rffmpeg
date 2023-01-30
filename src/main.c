#include "../include/libs.h"

int main(int argc, const char *argv[]) {
    char *arguments[MAX_ARGS];

    arguments[ARG_INPATH]        = calloc(PATHBUF, sizeof(char));
    arguments[ARG_INFORMAT]      = calloc(BUFFER, sizeof(char));
    arguments[ARG_INPARAMETERS]  = calloc(BUFFER, sizeof(char));
    arguments[ARG_OUTFORMAT]     = calloc(SHORTBUF, sizeof(char));
    arguments[ARG_NEWFOLDERNAME] = calloc(PATHBUF, sizeof(char));

    bool options[MAX_OPTS] = { false };

    char originalLocale[PATHBUF] ;
    strcpy_s(originalLocale, PATHBUF, setlocale(LC_ALL, ""));
    setlocale(LC_ALL, ".UTF8");  

    UINT originalConsoleOutputCP = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);

    processInfo_t processInformation = { 0 };
    errorCode_t exitCode = ERROR_NONE;
    DWORD originalConsoleMode;
    char originalConsoleWindowTitle[PATHBUF];
 
    inputMode_t inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

    enableVirtualTerminalProcessing(&originalConsoleMode);

    if (inputMode == CONSOLE) {
        GetConsoleTitleA(originalConsoleWindowTitle, PATHBUF);
        SetConsoleTitleA(consoleWindowTitle);
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

        getwchar();

        printf_s("\n");
        SetConsoleTitleA(originalConsoleWindowTitle);
    }

    setlocale(LC_ALL, originalLocale);

    SetConsoleOutputCP(originalConsoleOutputCP);
    resetConsoleMode(originalConsoleMode);

     return (int)exitCode;
}