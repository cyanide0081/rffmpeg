#include "../include/handlers.h"

bool _fileExists(const char *fileName);

int preventFilenameOverwrites(char *pureName, const char *fileFormat, const char *path) {
    size_t fullPathSize = snprintf(NULL, 0, "%s/%s.%s", path, pureName, fileFormat) + 1;

    char *fullPath = xcalloc(fullPathSize + strlen("-xxx"), sizeof(char));
    sprintf(fullPath, "%s/%s.%s", path, pureName, fileFormat);

    char newName[FILE_BUFFER];

    /* Keep appending indexes until it results in a unique file name */
    if (_fileExists(fullPath)) {
        size_t index = 0;

        while (_fileExists(fullPath))
            sprintf(fullPath, "%s/%s-%03zu.%s", path, pureName, ++index, fileFormat);  

        snprintf(newName, FILE_BUFFER, "%s-%03zu", pureName, index);
        memccpy(pureName, newName, '\0', FILE_BUFFER);
    }

    free(fullPath);

    return EXIT_SUCCESS;
}

int handleArgumentErrors(arguments *arguments) {
    /* Set current working directory as input path if none is provided */
    if (arguments->inPaths[0] == NULL) {
        arguments->inPaths[0] = getcwd(NULL, 0);
        
        #ifdef _WIN32
            GetCurrentDirectoryW(SHORTBUF, arguments->inPaths[0]);
        #endif
    }

    if (arguments->inFormats[0] == NULL) {
        printError("no input format (null)");
        return ERROR_NO_INPUT_FORMAT;
    }

    if (arguments->outFormat == NULL) {
        printError("no output format (null)");
        return ERROR_NO_OUTPUT_FORMAT;
    }

    return EXIT_SUCCESS;
}

bool _fileExists(const char *fileName) {
    struct stat statBuffer;
    return stat(fileName, &statBuffer) == 0 ? true : false;
}

int createTestProcess(void) {
    #ifdef _WIN32
        STARTUPINFOW ffmpegStartupInfo = { sizeof(ffmpegStartupInfo) };
        PROCESS_INFORMATION ffmpegProcessInfo;

        const char ffmpegProcessCall[] = "ffmpeg -loglevel quiet";

        if (CreateProcessW(NULL, ffmpegProcessCall, NULL, NULL,
        FALSE, 0, NULL, NULL, &ffmpegStartupInfo, &ffmpegProcessInfo)) {
            WaitForSingleObject(ffmpegProcessInfo.hProcess, INFINITE);
            CloseHandle(ffmpegProcessInfo.hProcess);
            CloseHandle(ffmpegProcessInfo.hThread);

            return EXIT_SUCCESS;
        }

        return EXIT_FAILURE;
    #else
        pid_t processID = fork();

        if (processID == 0) {
            execlp("ffmpeg", "ffmpeg", "-loglevel", "quiet", (char*)NULL);
            exit(errno);
        } else {
            int status;  
            waitpid(processID, &status, 0);

            int exitStatus = 0;
            
            if (WIFEXITED(status))
                exitStatus = WEXITSTATUS(status);

            /* Status 1 means the call succeeded and ffmpeg
            returned an error, and 2 means it wasn't found */
            if (exitStatus > 1)
                return EXIT_FAILURE;
        }
    #endif

    /* TODO: implement os-not-supported error message */

    return EXIT_SUCCESS;
}