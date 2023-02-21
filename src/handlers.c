#include "../include/handlers.h"

static bool _fileExists(const char *fileName);

int preventFilenameOverwrites(char *pureName, const char *fileFormat, const char *path) {
    size_t fullPathSize = snprintf(NULL, 0, "%s/%s.-xxx%s", path, pureName, fileFormat) + 1;

    char *fullPath = xcalloc(fullPathSize, sizeof(char));
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

int handleArgumentErrors(arguments *args) {
    /* Set current working directory as input path if none is provided */
    if (args->inPaths[0] == NULL) {
        #ifdef __linux__
            args->inPaths[0] = getcwd(NULL, 0);
        #elif defined _WIN32
            GetCurrentDirectoryW(PATH_BUFFER, args->inPaths[0]);
        #endif
    }

    if (args->ffOptions == NULL)
        args->ffOptions = strdup("");

    if (args->inFormats[0] == NULL) {
        printError("no input format", "null");
        return EXIT_FAILURE;
    }

    if (args->outFormat == NULL) {
        printError("no output format", "null");
        return EXIT_FAILURE;
    }

    if ((args->options & OPT_NEWFOLDER) && (strlen(args->customFolderName) >= FILE_BUFFER - 1)) {
        char *maxLength = asprintf("%d", FILE_BUFFER - 1);

        printError("custom folder name exceeds maximum allowed length", maxLength);
        free(maxLength);

        return EXIT_FAILURE;
    }

    if (args->options & OPT_NEWPATH) {
        if (args->customPathName == NULL) {
            printError("empty custom pathname field", "usage: --newpath=name");

            return EXIT_FAILURE;
        }

        if (strlen(args->customPathName) >= PATH_BUFFER) {
            char *maxLength = asprintf("%d", PATH_BUFFER - 1);

            printError("custom path name exceeds maximum allowed length", maxLength);
            free(maxLength);

            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
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
    #elif defined __linux__
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

        return EXIT_SUCCESS;
    #endif

    /* TODO: implement os-not-supported error message */

    return EXIT_FAILURE;
}

static bool _fileExists(const char *fileName) {
    struct stat statBuffer;
    return stat(fileName, &statBuffer) == 0 ? true : false;
}