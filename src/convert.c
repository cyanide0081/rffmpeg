#include <convert.h>

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

        #define PATH_SEPARATOR_CHAR '/'
        const char *fullPath = files[idx];
        const char *pathSeparator = (fullPath + strlen(fullPath) - 1);

        while (*pathSeparator != PATH_SEPARATOR_CHAR)
            pathSeparator--;

        char *filePath = strndup(fullPath, (pathSeparator - fullPath));
        char *baseName = strdup(pathSeparator + 1);
        assert(filePath);
        assert(baseName);

        char *outPath = NULL;
        char *newFolderName = (args->options & OPT_CUSTOMFOLDERNAME) ?
            args->customFolderName : args->outFormat;

        if (args->options & OPT_NEWFOLDER) {
            char *newPath = _asprintf("%s/%s", filePath, newFolderName);

            if (mkdir(newPath, S_IRWXU) != EXIT_SUCCESS && errno != EEXIST) {
                printErr("Unable to create subdirectory", strerror(errno));
                free(filePath);
                free(baseName);
                return EXIT_FAILURE;
            }

            outPath = newPath;
        } else if (args->options & OPT_NEWPATH) {
            char *newPath = strdup(args->customPathName);

            if (mkdir(newPath, S_IRWXU) != EXIT_SUCCESS && errno != EEXIST) {
                printErr("Unable to create new directory", strerror(errno));
                free(filePath);
                free(baseName);
                return EXIT_FAILURE;
            }

            outPath = newPath;
        } else {
            outPath = strdup(filePath);
        }

        char *fileNameNoExt = strdup(baseName);
        memset(fileNameNoExt + strlen(fileNameNoExt) - strlen(inputFormat) - 1,
               0, strlen(inputFormat) + 1);

        char *overwriteFlag = NULL;
        if (args->options & OPT_OVERWRITE) {
            overwriteFlag = "-y";
        } else {
            overwriteFlag = "";
            handleFileNameConflicts(fileNameNoExt, args->outFormat, outPath);
        }

        char *fullOutPath =
            _asprintf("%s/%s.%s", outPath, fileNameNoExt, args->outFormat);

        /* debug info printing */
        dprintf("FULLPATH:    \"%s\"\n",   fullPath);
        dprintf("FILEPATH:    \"%s\"\n",   filePath);
        dprintf("BASENAME:    \"%s\"\n",   baseName);
        dprintf("FULLOUTPATH: \"%s\"\n\n", fullOutPath);

#ifdef _WIN32
        size_t callBuf = UTF8toUTF16(ffmpegCall, -1, NULL, 0);
        wchar_t *ffmpegCallW = xcalloc(callBuf, sizeof(wchar_t));
        UTF8toUTF16(ffmpegCall, -1, ffmpegCallW, (int)callBuf);

        STARTUPINFOW ffmpegStartupInfo = { sizeof(ffmpegStartupInfo) };
        PROCESS_INFORMATION ffmpegProcessInfo;
        bool createdProcess = CreateProcessW(NULL, ffmpegCallW, NULL,
                                             NULL, FALSE, 0, NULL, NULL,
                                             &ffmpegStartupInfo,
                                             &ffmpegProcessInfo);

        free(ffmpegCallW);

        if (createdProcess == false) {
            DWORD err = GetLastError();
            wchar_t *errMsgW = NULL;
            int sizeW =
                FormatMessageW(
                               FORMAT_MESSAGE_ALLOCATE_BUFFER |
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
            trimSpaces(errMsg);

            printErr("call to FFmpeg failed", errMsg);
            LocalFree(errMsgW);
            free(errMsg);
        } else {
            WaitForSingleObject(ffmpegProcessInfo.hProcess, INFINITE);
            CloseHandle(ffmpegProcessInfo.hProcess);
            CloseHandle(ffmpegProcessInfo.hThread);

            stats->convertedFiles++;
        }
#else
        char *ffmpegCall =
            _asprintf("ffmpeg -hide_banner %s -i \"%s\" %s \"%s\"",
                      overwriteFlag, fullPath, args->ffOptions, fullOutPath);

        int systemCode = system(ffmpegCall);

        printf("\n");

        if (systemCode != EXIT_SUCCESS) {
            printErr("call to FFmpeg failed", strerror(systemCode));
            return systemCode;
        } else {
            stats->convertedFiles++;
        }

        free(ffmpegCall);
#endif

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

        free(fileNameNoExt);
        free(filePath);
        free(baseName);
        free(outPath);
    }

    return EXIT_SUCCESS;
}
