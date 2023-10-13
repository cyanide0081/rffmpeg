#include <convert.h>

extern Arena *globalArena;

static bool _fileExists(const char *fileName);
static int _handleFileNameConflicts(char *pureName,
                                    const char *fileFormat,
                                    const char *path);

int convertFiles(const char **files,
                 arguments *args,
                 processInfo *stats) {
    for (int idx = 0; files[idx]; idx++) {
        const char *inputFormat = NULL;

        for (int i = 0; args->inFormats[i]; i++) {
            if (strstr(files[idx], args->inFormats[i])) {
                inputFormat = args->inFormats[i];
                break;
            }
        }

        assert(inputFormat);

        const char *fullPath = files[idx];
        const char *pathDelimPoint = (fullPath + strlen(fullPath) - 1);

        while (*pathDelimPoint != PATH_SEP)
            pathDelimPoint--;

        char *filePath =
            GlobalArenaPushStringN(fullPath, (pathDelimPoint - fullPath));
        char *baseName = GlobalArenaPushString(pathDelimPoint + 1);

        assert(filePath);
        assert(baseName);

        char *outPath = filePath;
        const char *newFolderName = (args->options & OPT_CUSTOMFOLDERNAME) ?
            args->customFolder : args->outFormat;

        if (args->options & OPT_NEWFOLDER) {
            char *newPath = GlobalArenaSprintf("%s%c%s",
                                               filePath, PATH_SEP,
                                               newFolderName);

            if (mkdir(newPath, S_IRWXU) != EXIT_SUCCESS && errno != EEXIST) {
                printErr("Unable to create subdirectory", strerror(errno));
                return EXIT_FAILURE;
            }

            outPath = newPath;
        } else if (args->options & OPT_NEWPATH) {
            char *newPath = args->customPath;

            if (mkdir(newPath, S_IRWXU) != EXIT_SUCCESS && errno != EEXIST) {
                printErr("Unable to create new directory", strerror(errno));
                return EXIT_FAILURE;
            }

            outPath = newPath;
        }

        char *fileNameNoExt = strdup(baseName);
        memset(fileNameNoExt + strlen(fileNameNoExt) - strlen(inputFormat) - 1,
               0, strlen(inputFormat) + 1);

        const char *overwriteFlag = "-n";

        if (args->options & OPT_OVERWRITE) {
            overwriteFlag = "-y";
        } else {
            _handleFileNameConflicts(fileNameNoExt, args->outFormat, outPath);
        }

        char *fullOutPath =
            GlobalArenaSprintf("%s%c%s.%s",
                               outPath, PATH_SEP, fileNameNoExt, args->outFormat);

        /* debug info printing */
        dprintf("FULLPATH:    \"%s\"\n",   fullPath);
        dprintf("FILEPATH:    \"%s\"\n",   filePath);
        dprintf("BASENAME:    \"%s\"\n",   baseName);
        dprintf("FULLOUTPATH: \"%s\"\n\n", fullOutPath);
        dprintf("OUTPATH:     \"%s\"\n\n", args->customPath);

        char *ffmpegCall =
            GlobalArenaSprintf("ffmpeg -hide_banner %s -i \"%s\" %s \"%s\"",
                               overwriteFlag, fullPath,
                               args->ffOptions, fullOutPath);

#ifdef _WIN32
        int callBuf = UTF8toUTF16(ffmpegCall, -1, NULL, 0);
        wchar_t *ffmpegCallW = GlobalArenaPush(callBuf * sizeof(wchar_t));
        UTF8toUTF16(ffmpegCall, -1, ffmpegCallW, callBuf);

        STARTUPINFOW ffmpegStartupInfo = {0};
        PROCESS_INFORMATION ffmpegProcessInfo;
        bool createdProcess = CreateProcessW(NULL, ffmpegCallW, NULL,
                                             NULL, FALSE, 0, NULL, NULL,
                                             &ffmpegStartupInfo,
                                             &ffmpegProcessInfo);

        if (!createdProcess) {
            printWinErrMsg("call to FFmpeg failed", GetLastError());
        } else {
            WaitForSingleObject(ffmpegProcessInfo.hProcess, INFINITE);
            CloseHandle(ffmpegProcessInfo.hProcess);
            CloseHandle(ffmpegProcessInfo.hThread);

            stats->convertedFiles++;
        }
#else
        pid_t procId = fork();

        if (procId == 0) {
            /* NOTE: use system() for now cause i'm too lazy to build a
               dynamic array of args due to ffOptions right now */
            /* execlp("ffmpeg", "ffmpeg", overwriteFlag, "-i", */
            /*        fullPath, args->ffOptions, fullOutPath, (char*)NULL); */
            system(ffmpegCall);
            exit(errno);
        } else {
            int status;
            waitpid(procId, &status, 0);

            int exitStatus = 0;

            if (!WIFEXITED(status))
                exitStatus = WEXITSTATUS(status);

            if (exitStatus != 0) {
                char status[FILE_BUF];
                snprintf(status, FILE_BUF - 1, "exit status: %d", exitStatus);
                printErr("couldn't call ffmpeg", status);
                exit(EXIT_FAILURE);
            }

            stats->convertedFiles++;
        }
#endif
        printf("\n");

        if (args->options & OPT_CLEANUP) {
            if (remove(fullPath) != 0) {
                printErr("unable to delete original file", strerror(errno));
            } else {
                stats->deletedFiles++;
            }
        }

        /* Delete new folder in case it exists and no conversions succeeded */
        if (((args->options & OPT_NEWFOLDER) || (args->options & OPT_NEWPATH))
            && stats->convertedFiles == 0) {
            if (remove(outPath) != 0) {
                printErr("unable to delete unused directory", strerror(errno));
            }
        }
    }

    return EXIT_SUCCESS;
}

/* Appends 3-digit index to output filename in case it already exists */
static int _handleFileNameConflicts(char *pureName,
                                    const char *fileFormat,
                                    const char *path) {
    size_t fullPathSize = snprintf(NULL, 0, "%s%c%s.-xxx%s",
                                   path, PATH_SEP, pureName, fileFormat) + 1;

    char *fullPath = GlobalArenaPush(fullPathSize * sizeof(char));
    sprintf(fullPath, "%s%c%s.%s", path, PATH_SEP, pureName, fileFormat);

    char newName[FILE_BUF];

    /* Keep appending indexes until it results in a unique file name */
    if (_fileExists(fullPath)) {
        size_t index = 0;

        while (_fileExists(fullPath))
            sprintf(fullPath, "%s%c%s-%03ld.%s",
                    path, PATH_SEP, pureName, (long)++index, fileFormat);

        snprintf(newName, FILE_BUF, "%s-%03ld",
                 pureName, (long)index);
        memccpy(pureName, newName, '\0', FILE_BUF);
    }

    return EXIT_SUCCESS;
}

static bool _fileExists(const char *fileName) {
#ifdef _WIN32
    int len = UTF8toUTF16(fileName, -1, NULL, 0);
    wchar_t *fileNameW = GlobalArenaPush(len * sizeof(wchar_t));
    UTF8toUTF16(fileName, -1, fileNameW, len);

    WIN32_FIND_DATAW fileData;
    return FindFirstFileW(fileNameW, &fileData) != INVALID_HANDLE_VALUE;
#else /* POSIX */
    struct stat statBuffer;
    return stat(fileName, &statBuffer) == 0;
#endif
}
