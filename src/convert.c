#include <convert.h>

extern Arena *globalArena;

static __mt_call_conv _callFFmpeg(void *arg);
static size_t getNumberOfThreads(void);
static bool _fileExists(const char *fileName);
static int _checkFileName(char *name, const char *format, const char *path);

int convertFiles(const char **files, Arguments *args, ProcessInfo *stats) {
    char *outPath = NULL;

    size_t numberOfThreads = getNumberOfThreads();
    Thread *threads = GlobalArenaPush(numberOfThreads * sizeof(*threads));
    size_t fileIdx = 0;

    while (true) {
        for (size_t i = 0; i < numberOfThreads && files[fileIdx]; i++) {
            if (threads[i].handle) continue; // thread is busy (kiwi business)

            const char *inputFormat = NULL;

            for (int f = 0; args->inFormats[f]; f++) {
                if (strstr(files[fileIdx], args->inFormats[f])) {
                    inputFormat = args->inFormats[f];
                    break;
                }
            }

            assert(inputFormat);

            const char *fullPath = files[fileIdx];
            const char *pathDelimPoint = (fullPath + strlen(fullPath));

            while (*--pathDelimPoint != PATH_SEP);

            char *filePath =
                GlobalArenaPushStringN(fullPath, (pathDelimPoint - fullPath));
            char *baseName = GlobalArenaPushString(pathDelimPoint + 1);

            assert(filePath);
            assert(baseName);

            outPath = filePath;

            const char *newFolderName = (args->options & OPT_CUSTOMFOLDERNAME) ?
                args->outPath.customFolder : args->outFormat;

            if (args->options & OPT_NEWFOLDER) {
                char *newPath = GlobalArenaSprintf(
                    "%s%c%s", filePath, PATH_SEP, newFolderName
                );

                if (mkdir(newPath, S_IRWXU) != EXIT_SUCCESS && errno != EEXIST) {
                    printErr("Unable to create subdirectory", strerror(errno));
                    return EXIT_FAILURE;
                }

                outPath = newPath;
            } else if (args->options & OPT_NEWPATH) {
                char *newPath = args->outPath.customPath;

                if (mkdir(newPath, S_IRWXU) != EXIT_SUCCESS && errno != EEXIST) {
                    printErr("Unable to create new directory", strerror(errno));
                    return EXIT_FAILURE;
                }

                outPath = newPath;
            }

            char *fileNameNoExt =
                GlobalArenaPushStringN(
                    baseName, strlen(baseName) - strlen(inputFormat) - 1
                );

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
                GlobalArenaSprintf(
                    "ffmpeg -hide_banner -loglevel error "
                    "%s -i \"%s\" %s \"%s\"",
                    overwriteFlag, fullPath, args->ffOptions, fullOutPath
                );

            threads[i].targetFile =
                trimUTF8StringTo(fullPath + PREFIX_LEN, LINE_LEN - 30);
            threads[i].outFileID = fileIdx + 1;

            printf(
                " converting %sF-%.02zu %s-> %s\"%s\"%s to %s%s%s\n",
                COLOR_INPUT, threads[i].outFileID, COLOR_ACCENT, COLOR_INPUT,
                threads[i].targetFile, COLOR_DEFAULT, COLOR_ACCENT,
                args->outFormat, COLOR_DEFAULT
            );

#ifdef _WIN32
            int callBuf = UTF8toUTF16(ffmpegCall, -1, NULL, 0);
            wchar_t *ffmpegCallW = GlobalArenaPush(callBuf * sizeof(wchar_t));
            UTF8toUTF16(ffmpegCall, -1, ffmpegCallW, callBuf);

            threads[i].handle =
                (HANDLE)_beginthreadex(
                    NULL, 0, &_callFFmpeg,
                    ffmpegCallW, 0, NULL
                );

            if (!threads[i].handle) {
                printErr("unable to spawn new thread", strerror(errno));
                exit(errno);
            }

#else
            /* pthread setup stuff so we can get the thread's status later */
            pthread_mutex_init(&threads[i].mutex, NULL);
            pthread_cond_init(&threads[i].cond, NULL);

            int err = pthread_create(
                &threads[i].handle, NULL, &_callFFmpeg, ffmpegCall
            );

            if (err) {
                printErr("unable to spawn new thread", strerror(err));
                exit(err);
            }
#endif

            fileIdx += 1;
        }

        for (size_t i = 0; i < numberOfThreads; i++) {
#ifdef _WIN32
            if (
                WaitForSingleObject(threads[i].handle, 10) ==
                WAIT_TIMEOUT || !threads[i].handle
            ) continue;

            printf(
                " $ %sdone %sconverting %sF-%.02zu%s\n",
                COLOR_ACCENT, COLOR_DEFAULT, COLOR_INPUT,
                threads[i].outFileID, COLOR_DEFAULT
            );

            stats->convertedFiles += 1;

            CloseHandle(threads[i].handle);
#else
#error "ay we're not done yet"
            /* TODO: finish setting up condition and mutex
             * for the check to work properly down here */
            pthread_cond_t *cond = &threads[i].cond;
            pthread_mutex_t *mutex = &threads[i].mutex;
            struct timespec timeout;
            clock_gettime(CLOCK_REALTIME, &timeout);
            timeout.tv_nsec += 1e7; // 10ms

            /* TODO: finish figuring out how to properly setup the
             * condition and mutex stuff here so this works fr fr */
            int err = pthread_cond_timedwait(cond, mutex, &timeout);

            if (err == ETIMEDOUT) {
                continue;
            } else if (err == 0) {
                printf(
                    " ~ %sdone %sconverting %sF-%.02zu%s\n",
                    COLOR_ACCENT, COLOR_DEFAULT, COLOR_INPUT,
                    threads[i].outFileID, COLOR_DEFAULT
                );

                stats->convertedFiles += 1;
            } else {
                printErr("unable to retrieve thread status", strerror(err));
                exit(err);
            }
#endif

            memset(&threads[i], 0, sizeof(*threads));
        }

        /* NOTE: here we check if all threads are zeroed so
         * we only exit after all the work is actually done */
        if (isZeroMemory(threads, numberOfThreads * sizeof(*threads)) &&
            !files[fileIdx]) break;
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
    if (
        ((args->options & OPT_NEWFOLDER) || (args->options & OPT_NEWPATH)) &&
        stats->convertedFiles == 0
    ) {
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
    size_t n = (size_t)sysconf(_SC_NPROCESSORS_ONLN);
    return n > 2 ? n - 1 : 1;
#endif

    return *((volatile size_t*)0); // why are you here
}

static __mt_call_conv _callFFmpeg(void *arg) {
#ifdef _WIN32
    wchar_t *ffmpegCallW = (wchar_t*)arg;
    STARTUPINFOW ffmpegStartupInfo;
    PROCESS_INFORMATION ffmpegProcessInfo;

    bool createdProcess = CreateProcessW(
        NULL, ffmpegCallW, NULL, NULL, FALSE, 0, NULL, NULL,
        &ffmpegStartupInfo, &ffmpegProcessInfo
    );

    if (!createdProcess) {
        printWinErrMsg("unable to call FFmpeg", GetLastError());
         _endthreadex(-1);
        return -1;
    } else {
        WaitForSingleObject(ffmpegProcessInfo.hProcess, INFINITE);
        CloseHandle(ffmpegProcessInfo.hProcess);
        CloseHandle(ffmpegProcessInfo.hThread);
    }

    _endthreadex(0);
    return 0;
#else
    const char *ffmpegCall = arg;

    pid_t procId = fork();

    if (procId == 0) {
        /* NOTE: using system() for now cause i'm too lazy to
           build a dynamic array of args due to ffOptions */
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

static int _checkFileName(char *name, const char *format, const char *path) {
    size_t fullPathSize =
        snprintf(NULL, 0, "%s%c%s.-xxx%s", path, PATH_SEP, name, format) + 1;

    char *fullPath = GlobalArenaPush(fullPathSize * sizeof(char));
    sprintf(fullPath, "%s%c%s.%s", path, PATH_SEP, name, format);

    char newName[FILE_BUF];

    if (_fileExists(fullPath)) {
        size_t index = 0;

        while (_fileExists(fullPath)) {
            sprintf(
                fullPath, "%s%c%s-%03zu.%s", path,
                PATH_SEP, name, ++index, format
            );
        }

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
