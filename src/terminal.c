#include "../include/terminal.h"

void printError(const char *message, const char *descriptor) {
    fprintf(stderr, "%sERROR: %s%s (%s%s%s)\n\n",
     CHARCOLOR_RED,
     CHARCOLOR_WHITE, message,
     CHARCOLOR_RED, descriptor,
     COLOR_DEFAULT);
}

void displayEndDialog(processInfo *processInformation) {
    if (processInformation->convertedFiles == 0) {
        printError("no input files were found", "check if your entered directory is correct");
    } else {
        fmtTime executionTime = formatTime(processInformation->executionTime);

        printf(" %sDONE!%s\n", CHARCOLOR_RED, COLOR_DEFAULT);
        printf("\n");
        printf(" %sProcessed files: %s%llu%s\n", CHARCOLOR_WHITE, CHARCOLOR_RED, 
         (uint64_t)processInformation->convertedFiles, COLOR_DEFAULT);
        printf(" %sDeleted files:   %s%llu%s\n", CHARCOLOR_WHITE, CHARCOLOR_RED, 
         (uint64_t)processInformation->deletedFiles, COLOR_DEFAULT);
        printf(" %sElapsed time:    %s%02llu:%02llu:%05.2lf%s\n",
         CHARCOLOR_WHITE, CHARCOLOR_RED,
         executionTime.hours, executionTime.minutes, executionTime.seconds,
         COLOR_DEFAULT);
        printf("\n");
    }
}

#ifdef _WIN32
    int restoreConsoleMode(DWORD originalConsoleMode) {
        HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

        if (SetConsoleMode(handleToStdOut, originalConsoleMode == false))
            return GetLastError();
        
        return EXIT_SUCCESS;
    }

    int enableVirtualTerminalProcessing(PDWORD originalConsoleMode) {
        HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD consoleMode = 0;

        if (GetConsoleMode(handleToStdOut, &consoleMode) == false)
            return GetLastError();

        *originalConsoleMode = consoleMode;
        consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        if (SetConsoleMode(handleToStdOut, consoleMode) == false)
            return GetLastError();

        return EXIT_SUCCESS;
    }
#endif // _WIN32