#include <convert.h>

extern Arena *globalArena;

typedef struct Thread {
    char *targetFile;
    char *outFile;
    size_t outFileID;

#ifdef _WIN32
    HANDLE handle;
#else
    /* TODO: POSIX */
#endif
} Thread;

static unsigned __stdcall _callFFmpeg(void *arg);
static size_t getNumberOfThreads(void);
static bool _fileExists(const char *fileName);
static int _checkFileName(char *name, const char *format, const char *path);

int convertFiles(const char **files, arguments *args, processInfo *stats) {
    char *outPath = NULL;

    size_t numberOfThreads = getNumberOfThreads();
    Thread *threads = GlobalArenaPush(numberOfThreads * sizeof(Thread));

    size_t fileIdx = 0;

    while (true) {
        for (size_t threadIdx = 0;
             threadIdx < numberOfThreads && files[fileIdx];
             threadIdx++
            ) {
            if (threads[threadIdx].handle) continue; // Thread is busy

            const char *inputFormat = NULL;

            for (int i = 0; args->inFormats[i]; i++) {
                if (strstr(files[fileIdx], args->inFormats[i])) {
                    inputFormat = args->inFormats[i];
                    break;
                }
            }

            assert(inputFormat);

            const char *fullPath = files[fileIdx];
            const char *pathDelimPoint = (fullPath + strlen(fullPath) - 1);

            while (*pathDelimPoint-- != PATH_SEP);

            char *filePath =
                GlobalArenaPushStringN(fullPath, (pathDelimPoint - fullPath));
            char *baseName = GlobalArenaPushString(pathDelimPoint + 1);

            assert(filePath);
            assert(baseName);

            outPath = filePath;

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

            char *fileNameNoExt =
                GlobalArenaPushStringN(baseName, strlen(baseName) -
                                       strlen(inputFormat) - 1);

            const char *overwriteFlag = "-n";

            if (args->options & OPT_OVERWRITE) {
                overwriteFlag = "-y";
            } else {
                _checkFileName(fileNameNoExt, args->outFormat, outPath);
            }

            char *fullOutPath =
                GlobalArenaSprintf("%s%c%s.%s", outPath, PATH_SEP,
                                   fileNameNoExt, args->outFormat);

            char *ffmpegCall =
                GlobalArenaSprintf("ffmpeg -hide_banner -loglevel error "
                                   "%s -i \"%s\" %s \"%s\"",
                                   overwriteFlag, fullPath,
                                   args->ffOptions, fullOutPath);

#ifdef _WIN32
            int callBuf = UTF8toUTF16(ffmpegCall, -1, NULL, 0);
            wchar_t *ffmpegCallW = GlobalArenaPush(callBuf * sizeof(wchar_t));
            UTF8toUTF16(ffmpegCall, -1, ffmpegCallW, callBuf);

            threads[threadIdx].targetFile =
                trimUTF8StringTo(fullPath + PREFIX_LEN, LINE_LEN - 30);
            threads[threadIdx].outFile =
                trimUTF8StringTo(fullOutPath + PREFIX_LEN, LINE_LEN - 30);
            threads[threadIdx].outFileID = fileIdx + 1;

            printf(" converting %sF-%.02zu %s-> %s\"%s\"%s to %s%s%s\n",
                   COLOR_INPUT, threads[threadIdx].outFileID,
                   COLOR_ACCENT, COLOR_INPUT,
                   threads[threadIdx].targetFile, COLOR_DEFAULT,
                   COLOR_ACCENT, args->outFormat, COLOR_DEFAULT);

            threads[threadIdx].handle =
                (HANDLE)_beginthreadex(NULL, 0, &_callFFmpeg,
                                       ffmpegCallW, 0, NULL);

            if (!threads[threadIdx].handle) {
                printErr("unable to spawn new thread", strerror(errno));
            }

            fileIdx += 1;
#else
            /* TODO: replace this POSIX part with multithreaded code! */

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
                    printErr("unable to call FFmpeg", status);
                    exit(EXIT_FAILURE);
                }
            }
#endif
        }

        for (size_t i = 0; i < numberOfThreads; i++) {
#ifdef _WIN32
            if (WaitForSingleObject(threads[i].handle, 10) == WAIT_TIMEOUT ||
                !threads[i].handle) continue;

            printf(" ~ %sdone %sconverting %sF-%.02zu%s\n",
                   COLOR_ACCENT, COLOR_DEFAULT, COLOR_INPUT,
                   threads[i].outFileID, COLOR_DEFAULT);

            stats->convertedFiles += 1;

            CloseHandle(threads[i].handle);
#else
            /* TODO: POSIX impl */
#endif
            memset(&threads[i], 0, sizeof(*threads));
        }

        /* NOTE: here we check if all threads are nulled so
         * we only exit after all the work is actually done */
        if (!files[fileIdx] &&
            isZeroMemory(threads, numberOfThreads * sizeof(Thread))) break;
    }

    putchar('\n');

    for (int i = 0; files[i]; i++) {
        if (args->options & OPT_CLEANUP) {
            if (remove(files[i]) != 0) {
                printErr("unable to delete original file", strerror(errno));
            } else {
                stats->deletedFiles += 1;
            }
        }
    }

    /* Delete new folder in case it exists and no conversions succeeded */
    if (((args->options & OPT_NEWFOLDER) || (args->options & OPT_NEWPATH))
        && stats->convertedFiles == 0) {
        if (remove(outPath) != 0) {
            printErr("unable to delete unused directory", strerror(errno));
        }
    }

    return EXIT_SUCCESS;
}

static size_t getNumberOfThreads(void) {
#ifdef _WIN32
    SYSTEM_INFO sysInfo = {0};
    GetSystemInfo(&sysInfo);

    /* NOTE: - 1 since we already have a main thread (I guess?) */
    return sysInfo.dwNumberOfProcessors > 2 ?
        (size_t)sysInfo.dwNumberOfProcessors - 1 : 1;
#else
    /* TODO: POSIX impl */
#endif

    return *((volatile size_t*)0); // why are you here
}

static unsigned __stdcall _callFFmpeg(void *arg) {
#ifdef _WIN32
    wchar_t *ffmpegCallW = (wchar_t*)arg;
    STARTUPINFOW ffmpegStartupInfo = {0};
    PROCESS_INFORMATION ffmpegProcessInfo;

    bool createdProcess = CreateProcessW(NULL, ffmpegCallW, NULL,
                                         NULL, FALSE, 0, NULL, NULL,
                                         &ffmpegStartupInfo,
                                         &ffmpegProcessInfo);

    if (!createdProcess) {
        printWinErrMsg("unable to call FFmpeg", GetLastError());
        _endthreadex(-1);
        return -1;
    } else {
        WaitForSingleObject(ffmpegProcessInfo.hProcess, INFINITE);
        CloseHandle(ffmpegProcessInfo.hProcess);
        CloseHandle(ffmpegProcessInfo.hThread);
    }
#else
    /* TODO: POSIX impl */
#endif

    _endthreadex(0);
    return 0;
}

static int _checkFileName(char *name, const char *format, const char *path) {
    size_t fullPathSize = snprintf(NULL, 0, "%s%c%s.-xxx%s",
                                   path, PATH_SEP, name, format) + 1;

    char *fullPath = GlobalArenaPush(fullPathSize * sizeof(char));
    sprintf(fullPath, "%s%c%s.%s", path, PATH_SEP, name, format);

    char newName[FILE_BUF];

    /* Keep appending indexes until it results in a unique file name */
    if (_fileExists(fullPath)) {
        size_t index = 0;

        while (_fileExists(fullPath))
            sprintf(fullPath, "%s%c%s-%03zu.%s",
                    path, PATH_SEP, name, ++index, format);

        snprintf(newName, FILE_BUF, "%s-%03zu", name, index);
        memccpy(name, newName, '\0', FILE_BUF);
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
