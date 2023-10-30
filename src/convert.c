#include <convert.h>

extern Arena *globalArena;

static __mt_call_conv _callFFmpeg(void *arg);
static int _formatFileName(char *name, const char *ext, const char *path);
static inline void _updateProgBar(Arguments *args, ProcessInfo *stats);
static inline void _clearProgBar(void);
static inline bool _fileExists(const char *fileName);

int convertFiles(const char **files, Arguments *args, ProcessInfo *stats) {
#ifndef _WIN32
    pthread_attr_t attr;
    threadAttrInit(&attr);
#endif

    char *outPath = NULL;
    size_t numberOfThreads = args->numberOfThreads ?
        args->numberOfThreads : getNumberOfOnlineThreads();
    Thread *threads = GlobalArenaPush(numberOfThreads * sizeof(*threads));
    size_t fileIdx = 0;

    do {
        for (size_t i = 0; i < numberOfThreads && files[fileIdx]; i++) {
            if (threads[i].handle) continue; // thread is busy (kiwi business)

            _updateProgBar(args, stats);

            const char *inputFormat = NULL;

            for (int f = 0; args->inFormats[f]; f++) {
                if (strstr(files[fileIdx], args->inFormats[f])) {
                    inputFormat = args->inFormats[f];
                    break;
                }
            }

            const char *fullPath = files[fileIdx];
            const char *pathDelimPoint = (fullPath + strlen(fullPath));

            while (*--pathDelimPoint != PATH_SEP);

            char *filePath = GlobalArenaPushStringN(
                fullPath, (pathDelimPoint - fullPath)
            );
            char *baseName = GlobalArenaPushString(pathDelimPoint + 1);

            outPath = filePath;

            const char *newFolderName = (args->options & OPT_CUSTOMFOLDERNAME) ?
                args->outPath.customFolder : args->outFormat;

            if (args->options & OPT_NEWFOLDER || args->options & OPT_NEWPATH) {
                char *newPath =
                    args->options & OPT_NEWFOLDER ? GlobalArenaSprintf(
                        "%s%c%s", filePath, PATH_SEP, newFolderName
                    ) : args->outPath.customPath;

                if (mkdir(newPath, S_IRWXU) != EXIT_SUCCESS && errno != EEXIST) {
                    printErr("unable to create subdirectory", strerror(errno));
                    return EXIT_FAILURE;
                }

                outPath = newPath;
            }

            char *fileNameNoExt = GlobalArenaPushStringN(
                baseName, strlen(baseName) - strlen(inputFormat) - 1
            );

            const char *overwriteFlag = "-n";

            if (args->options & OPT_OVERWRITE) {
                overwriteFlag = "-y";
            } else {
                _formatFileName(fileNameNoExt, args->outFormat, outPath);
            }

            char *fullOutPath = GlobalArenaSprintf(
                "%s%c%s.%s", outPath, PATH_SEP, fileNameNoExt, args->outFormat
            );

            char *ffmpegCall = GlobalArenaSprintf(
                "ffmpeg -nostdin -hide_banner -loglevel error "
                "%s -i \"%s\" %s \"%s\"",
                overwriteFlag, fullPath, args->ffOptions, fullOutPath
            );

            threads[i].callArg = ffmpegCall;
            threads[i].targetFile = trimUTF8StringTo(
                fullPath + PREFIX_LEN, LINE_LEN - 36
            );
            threads[i].outFileID = fileIdx + 1;

#ifdef _WIN32
            threads[i].handle = (HANDLE)_beginthreadex(
                NULL, 0, &_callFFmpeg, &threads[i], 0, NULL
            );

            if (!threads[i].handle) {
                printErr("unable to spawn new thread", strerror(errno));
                exit(errno);
            }
#else
            pthread_mutex_init(&threads[i].mutex, NULL);
            pthread_cond_init(&threads[i].cond, NULL);

            int err = pthread_create(
                &threads[i].handle, &attr, &_callFFmpeg, &threads[i]
            );

            if (err) {
                printErr("unable to spawn new thread", strerror(err));
                exit(err);
            }
#endif

            fileIdx += 1;
        }

        for (size_t i = 0; i < numberOfThreads; i++) {
            if (!threads[i].handle) continue;

#ifdef _WIN32
            if (
                WaitForSingleObject(threads[i].handle, TIMEOUT_MS) ==
                WAIT_TIMEOUT
            ) continue;

            CloseHandle(threads[i].handle);
#else
            struct timespec time;
            clock_gettime(CLOCK_REALTIME, &time);
            time.tv_nsec += (TIMEOUT_MS * 1e6);
            /* NOTE: making sure tv_nsec doesn't exceed it's maximum value */
            if (time.tv_nsec > TV_NSEC_MAX) time.tv_nsec = TV_NSEC_MAX;

            int wait = pthread_cond_timedwait(
                &threads[i].cond, &threads[i].mutex, &time
            );

            if (wait == ETIMEDOUT) continue;

            int err = pthread_join(threads[i].handle, NULL);

            if (err) {
                printErr("unable to join threads", strerror(err));
                exit(err);
            }

            if ((err = pthread_cond_destroy(&threads[i].cond))) {
                printErr("unable to destroy condition", strerror(err));
                abort();
            }

            if ((err = pthread_mutex_destroy(&threads[i].mutex))) {
                printErr("unable to destroy mutex", strerror(err));
                abort();
            }
#endif

            stats->convertedFiles += 1;

            _updateProgBar(args, stats);

            memset(&threads[i], 0, sizeof(*threads));
        }
    } while (
        !isZeroMemory(threads, numberOfThreads * sizeof(*threads)) ||
        files[fileIdx]
    );

#ifndef _WIN32
    threadAttrDestroy(&attr);
#endif

    putchar('\n');

    for (int i = 0; files[i]; i++) {
        if (args->options & OPT_CLEANUP) {
            if (remove(files[i])) {
                printErr("unable to delete original file", strerror(errno));
            } else {
                stats->deletedFiles += 1;
            }
        }
    }

    if (
        ((args->options & OPT_NEWFOLDER) ||
         (args->options & OPT_NEWPATH)) &&
        stats->convertedFiles == 0
    ) {
        if (remove(outPath)) {
            printErr("unable to delete unused directory", strerror(errno));
        }
    }

    return 0;
}

static __mt_call_conv _callFFmpeg(void *arg) {
    Thread *thread = (Thread*)arg;
    const char *ffmpegCall = thread->callArg;

#ifdef _WIN32
    int callBuf = UTF8toUTF16(ffmpegCall, -1, NULL, 0);
    wchar_t *ffmpegCallW = GlobalArenaPush(callBuf * sizeof(wchar_t));
    UTF8toUTF16(ffmpegCall, -1, ffmpegCallW, callBuf);

    STARTUPINFOW ffmpegStartupInfo = {0};
    PROCESS_INFORMATION ffmpegProcessInfo = {0};

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
    pid_t procId = fork();

    if (procId == 0) {
        /* NOTE: using system() for now cause i'm too lazy
         * to build a dynamic array of args due to ffOptions
         * having to be popped from the array in case it's
         * an empty string (._.)  */
        int err = system(ffmpegCall);
        exit(err);
    } else {
        int status;
        waitpid(procId, &status, 0);

        int exitStatus = 0;

        if (!WIFEXITED(status))
            exitStatus = WEXITSTATUS(status);

        if (exitStatus != 0) {
            char status[FILE_BUF];
            snprintf(status, FILE_BUF, "exit status: %d", exitStatus);
            printErr("unable to call FFmpeg", status);
            exit(exitStatus);
        }
    }

    static struct timespec timeout = { .tv_nsec = TIMEOUT_MS * 1e7 / 2 };

    while (pthread_mutex_trylock(&thread->mutex) == EBUSY)
        nanosleep(&timeout, NULL);

    pthread_cond_broadcast(&thread->cond);
    pthread_mutex_unlock(&thread->mutex);

    return NULL;
#endif
}

static int _formatFileName(char *name, const char *ext, const char *path) {
    size_t fullPathSize =
        snprintf(NULL, 0, "%s%c%s.-xxx%s", path, PATH_SEP, name, ext) + 1;

    char *fullPath = GlobalArenaPush(fullPathSize * sizeof(char));
    sprintf(fullPath, "%s%c%s.%s", path, PATH_SEP, name, ext);

    char newName[FILE_BUF];

    if (_fileExists(fullPath)) {
        size_t index = 0;

        while (_fileExists(fullPath)) {
            sprintf(
                fullPath, "%s%c%s-%03zu.%s", path,
                PATH_SEP, name, ++index, ext
            );
        }

        snprintf(newName, FILE_BUF, "%s-%03zu", name, index);
        strncpy(name, newName, FILE_BUF);
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

#define PROGBAR_LINES 1
#define BAR_LEN (LINE_LEN - 21)

enum ProgBarStatus {
    CLEARED,
    VISIBLE
} progBarStatus = CLEARED;

/* NOTE: progress bar/stats printing function (style subject to change) */
static inline void _updateProgBar(Arguments *args, ProcessInfo *stats) {
    (void)args; // will use these later

    if (progBarStatus == VISIBLE) _clearProgBar();

    char progBar[LINE_LEN * PROGBAR_LINES] = {0};
    size_t progBarIdx = 0;

    progBarIdx += sprintf(
        progBar, " Progress: [%s", COLOR_ACCENT
    );

    size_t fill = (BAR_LEN * stats->convertedFiles) / stats->totalFiles;

    for (size_t i = 0; i < BAR_LEN; i++) {
        if (i < fill) {
            progBar[progBarIdx++] = '#';
        } else {
            progBar[progBarIdx++] = ' ';
        }
    }

    sprintf(
        (progBar + progBarIdx), "%s] %6.2f%%", COLOR_DEFAULT,
        (double)(100.0F * stats->convertedFiles / stats->totalFiles)
    );

    puts(progBar);
    progBarStatus = VISIBLE;
}

static inline void _clearProgBar(void) {
    for (size_t i = 0; i < PROGBAR_LINES; i++)
        printf(LINE_ERASE LINE_MOVE_UP);

    printf(LINE_ERASE CARRIAGE_RET);

    progBarStatus = CLEARED;
}
