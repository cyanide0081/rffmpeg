#include "../lib/handlers.h"

static bool _fileExists(const char *fileName);

/* Appends 3-digit index to output filename in case it already exists */
int handleFileNameConflicts(char *pureName,
                            const char *fileFormat,
                            const char *path) {
    size_t fullPathSize =
        snprintf(NULL, 0, "%s/%s.-xxx%s", path, pureName, fileFormat) + 1;

    char *fullPath = xcalloc(fullPathSize, sizeof(char));
    sprintf(fullPath, "%s/%s.%s", path, pureName, fileFormat);

    char newName[FILE_BUFFER];

    /* Keep appending indexes until it results in a unique file name */
    if (_fileExists(fullPath)) {
        size_t index = 0;

        while (_fileExists(fullPath))
            sprintf(fullPath, "%s/%s-%03" PRIu64 ".%s",
             path, pureName, (uint64_t)++index, fileFormat);

        snprintf(newName, FILE_BUFFER, "%s-%03" PRIu64,
                 pureName, (uint64_t)index);
        memccpy(pureName, newName, '\0', FILE_BUFFER);
    }

    free(fullPath);

    return EXIT_SUCCESS;
}

/* Handles edge cases regarding arguments as well as errors */
int handleArgErrors(arguments *args) {
    int code = EXIT_SUCCESS;

    /* Set current working directory as input path if none is provided */
    if (args->inPaths[0] == NULL) {
#ifdef _WIN32
        wchar_t currentDirW[PATH_BUFFER];
        GetCurrentDirectoryW(PATH_BUFFER, currentDirW);
        char currentDir[PATH_BUFFER];
        utf16toutf8(currentDirW, -1, currentDir, PATH_BUFFER);

        args->inPaths[0] = _strdup(currentDir);
#else
        char currentDir[PATH_BUFFER];
        getcwd(currentDir, PATH_BUFFER);

        args->inPaths[0] = strdup(currentDir);
#endif
    }

    if (args->ffOptions == NULL)
        args->ffOptions = strdup("");

    if (args->inFormats[0] == NULL || *args->inFormats[0] == '\0') {
        printerr("no input format", "null");

        code = EXIT_FAILURE;
    }

    if (args->outFormat == NULL || *args->outFormat == '\0') {
        printerr("no output format", "null");

        code = EXIT_FAILURE;
    }

    if ((args->options & OPT_NEWFOLDER)
        && (strlen(args->customFolderName) >= FILE_BUFFER - 1)) {
        char *maxLength = _asprintf("%d", FILE_BUFFER - 1);

        printerr("custom folder name exceeds maximum allowed length",
                   maxLength);
        free(maxLength);

        code = EXIT_FAILURE;
    }

    if (args->options & OPT_NEWPATH) {
        if (args->customPathName == NULL) {
            printerr("empty custom pathname field", "usage: --newpath=name");

            code = EXIT_FAILURE;
        } else if (strlen(args->customPathName) >= PATH_BUFFER) {
            char *maxLength = _asprintf("%d", PATH_BUFFER - 1);

            printerr("custom path name exceeds maximum allowed length",
                     maxLength);
            free(maxLength);

            code = EXIT_FAILURE;
        }
    }

    for (int i = 0; args->inFormats[i] != NULL; i++) {
        if (strcmp(args->inFormats[i], args->outFormat) == 0
            && !(args->options & OPT_NEWFOLDER)
            && !(args->options & OPT_NEWPATH)) {
            printerr("can't use ffmpeg with identical input \
                       and output formats",
                       "use '--newpath' or '--newfolder' \
                       to save the files in a new directory");

            code = EXIT_FAILURE;
            break;
        }
    }

    return code;
}

int createTestProcess(void) {
#ifdef _WIN32
    STARTUPINFOW ffmpegStartupInfo = { sizeof(ffmpegStartupInfo) };
    PROCESS_INFORMATION ffmpegProcessInfo;

    wchar_t ffmpegProcessCall[] = u"ffmpeg -loglevel quiet";

    if (CreateProcessW(NULL, ffmpegProcessCall, NULL, NULL, FALSE, 0,
                       NULL, NULL, &ffmpegStartupInfo, &ffmpegProcessInfo)) {
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

    return EXIT_SUCCESS;
#endif
}

static bool _fileExists(const char *fileName) {
#ifdef _WIN32
    wchar_t fileNameW[PATH_BUFFER];
    utf8toutf16(fileName, -1, fileNameW, PATH_BUFFER);

    WIN32_FIND_DATAW fileData;

    return FindFirstFileW(fileNameW, &fileData) !=
        INVALID_HANDLE_VALUE ? true : false;
#else /* Unix */
    struct stat statBuffer;
    return stat(fileName, &statBuffer) == 0 ? true : false;
#endif
}
