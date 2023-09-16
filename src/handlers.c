#include <handlers.h>

static bool _fileExists(const char *fileName);

/* Appends 3-digit index to output filename in case it already exists */
int handleFileNameConflicts(char *pureName,
                            const char *fileFormat,
                            const char *path) {
    size_t fullPathSize =
        snprintf(NULL, 0, "%s/%s.-xxx%s", path, pureName, fileFormat) + 1;

    char *fullPath = xcalloc(fullPathSize, sizeof(char));
    sprintf(fullPath, "%s/%s.%s", path, pureName, fileFormat);

    char newName[NAME_MAX];

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
        int len = GetCurrentDirectoryW(NULL, 0);
        wchar_t *currentDirW = xcalloc(len, sizeof(wchar_t));
        GetCurrentDirectoryW((DWORD)len, currentDirW);

        len = UTF16toUTF8(currentDirW, -1, NULL, 0);
        args->inPaths[0] = xcalloc(len, sizeof(char));

        UTF16toUTF8(currentDirW, -1, args->inPaths[0], len);
        free(currentDirW);
#else
        if (!(args->inPaths[0] = getcwd(NULL, 0))) {
            printErr("couldn't retrieve current working directory",
                     strerror(errno));
            exit(errno);
        }
#endif
    }

    if (args->ffOptions == NULL)
        args->ffOptions = strdup("");

    if (args->inFormats[0] == NULL || *args->inFormats[0] == '\0') {
        printErr("no input format", "NULL");
        code = EXIT_FAILURE;
    }

    if (args->outFormat == NULL || *args->outFormat == '\0') {
        printErr("no output format", "NULL");
        code = EXIT_FAILURE;
    }

    if ((args->options & OPT_NEWFOLDER)
        && (strlen(args->customFolderName) >= NAME_MAX - 1)
        ) {
        char *maxLen = _asprintf("%d", NAME_MAX - 1);
        printErr("custom folder name exceeds maximum allowed length", maxLen);
        free(maxLen);

        code = EXIT_FAILURE;
    }

    if (args->options & OPT_NEWPATH) {
        if (args->customPathName == NULL) {
            printErr("empty custom pathname field", "usage: --newpath=name");

            code = EXIT_FAILURE;
        } /* else if (strlen(args->customPathName) >= PATH_BUFFER) { */
        /*     char *maxLength = _asprintf("%d", PATH_BUFFER - 1); */

        /*     printErr("custom path name exceeds maximum allowed length", */
        /*              maxLength); */
        /*     free(maxLength); */

        /*     code = EXIT_FAILURE; */
        /* } */

        /* NOTE: maybe reawake this code for windows only sice its paths
           can't exceed 260 bytes by default (maybe also prompt the user
           in case they want to remove the limitation 8) */
    }

    for (int i = 0; args->inFormats[i] != NULL; i++) {
        if (strcmp(args->inFormats[i], args->outFormat) == 0
            && !(args->options & OPT_NEWFOLDER)
            && !(args->options & OPT_NEWPATH)
            ) {
            printErr("can't use ffmpeg with identical input \
                       and output formats",
                       "use '--newpath' or '--newfolder' \
                       to save the files in a new directory");

            code = EXIT_FAILURE;
            break;
        }
    }

    return code;
}

void createTestProcess(void) {
#ifdef _WIN32
    STARTUPINFOW ffmpegStartupInfo = { sizeof(ffmpegStartupInfo) };
    PROCESS_INFORMATION ffmpegProcessInfo;
    wchar_t ffmpegProcessCall[] = u"ffmpeg -loglevel quiet";

    if (CreateProcessW(NULL, ffmpegProcessCall, NULL, NULL, FALSE, 0,
                       NULL, NULL, &ffmpegStartupInfo, &ffmpegProcessInfo)) {
        WaitForSingleObject(ffmpegProcessInfo.hProcess, INFINITE);
        CloseHandle(ffmpegProcessInfo.hProcess);
        CloseHandle(ffmpegProcessInfo.hThread);

        return;
    }

    DWORD err = GetLastError();
    wchar_t *errMsgW = NULL;
    int sizeW = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                 FORMAT_MESSAGE_FROM_SYSTEM |
                                 FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL,
                                 err,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 (LPWSTR)&errMsgW,
                                 0,
                                 NULL);

    int size = UTF16toUTF8(errMsgW, (int)sizeW, NULL, 0);
    char *errMsg = xcalloc(size, sizeof(char));
    UTF16toUTF8(errMsgW, sizeW, errMsg, size);

    printErr("couldn't start ffmpeg", errMsg);
    LocalFree(errMsgW);
    free(errMsg);
    exit(err);
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
           returned an error, and 2 means it wasn't found
           TODO: handle more exit codes here! */
        if (exitStatus > 1) {
            printErr("couldn't start ffmpeg", "binary not found");
            exit(EXIT_FAILURE);
        }
    }

    return;
#endif
}

static bool _fileExists(const char *fileName) {
#ifdef _WIN32
    int len = UTF8toUTF16(fileName, -1, NULL, 0);
    wchar_t *fileNameW = xcalloc(len, sizeof(wchar_t));
    UTF8toUTF16(fileName, -1, fileNameW, len);
    WIN32_FIND_DATAW fileData;

    bool result = FindFirstFileW(fileNameW, &fileData) !=
        INVALID_HANDLE_VALUE ? true : false;

    free(fileNameW);
    return result;
#else /* *NIXES */
    struct stat statBuffer;
    return stat(fileName, &statBuffer) == 0 ? true : false;
#endif
}
