#include "../include/libs.h"
#include "../include/headers.h"

int main(int argc, char *argv[]) {
    #ifdef _WIN32
        #define UNICODE
        #define _UNICODE

        /* Setup Unicode (UTF-16LE) console I/O for Windows */
        setmode(_fileno(stdout), _O_U16TEXT); 

        /* Enable virtual terminal sequences for colored console output */
        DWORD originalConsoleMode;
        char originalConsoleWindowTitle[PATH_BUFFER];
        enableVirtualTerminalProcessing(&originalConsoleMode);

        if (inputMode == CONSOLE) {
            GetConsoleTitleW(originalConsoleWindowTitle, PATH_BUFFER);
            SetConsoleTitleW(CONSOLE_WINDOW_TITLE);
        }
    #endif

    processInfo processInformation = { 0 };
    inputMode inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

    printf("%s%s%s\n\n", CHARCOLOR_RED, FULL_PROGRAM_TITLE, COLOR_DEFAULT);
    
    int exitCode = createTestProcess();

    if (exitCode == EXIT_FAILURE) {  
        printError("couldn't find FFmpeg", strerror(errno));
        exit(EXIT_FAILURE);
    }

    arguments *parsedArguments = initializeArguments();

    if (inputMode == ARGUMENTS) {
        parseArgs(argc, argv, parsedArguments);
    } else {
        parseConsoleInput(parsedArguments);
    }

    if (parsedArguments->options & OPT_DISPLAYHELP && inputMode == ARGUMENTS) {
        displayHelp();
    } else if ((exitCode = handleArgErrors(parsedArguments)) == EXIT_SUCCESS) {
        struct timespec startTime, endTime;
        clock_gettime(CLOCK_MONOTONIC_RAW, &startTime);

        exitCode = searchDirs(parsedArguments, &processInformation);

        clock_gettime(CLOCK_MONOTONIC_RAW, &endTime);
        
        processInformation.executionTime = 
         (double)(endTime.tv_sec - startTime.tv_sec) + (endTime.tv_nsec - startTime.tv_nsec) / 1e9;

        if (exitCode == EXIT_SUCCESS)
            displayEndDialog(&processInformation);
    }

    if (inputMode == CONSOLE) {
        printf(" %s(Press any key to exit) %s", CHARCOLOR_WHITE, COLOR_DEFAULT);
        getchar();

        printf("\n");
    }

    #ifdef _WIN32
        if (inputMode == CONSOLE)
            SetConsoleTitleW(originalConsoleWindowTitle);
        
        restoreConsoleMode(originalConsoleMode);
    #endif

    destroyArguments(parsedArguments);

    return exitCode;
}