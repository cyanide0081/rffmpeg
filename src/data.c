#include <data.h>

extern Arena *globalArena;

Arguments *ArgumentsAlloc(void) {
    Arguments *args = GlobalArenaPush(sizeof(*args));
    args->inPaths   = GlobalArenaPush(LIST_BUF * sizeof(char*));
    args->inFormats = GlobalArenaPush(LIST_BUF * sizeof(char*));

    return args;
}

FmtTime formatTime(double seconds) {
    FmtTime time = {
        .hours   = (size_t)(seconds / 3600),
        .minutes = (size_t)((seconds - (time.hours * 3600)) / 60),
        .seconds = (double)(seconds - (time.hours * 3600) - (time.minutes * 60))
    };

    return time;
}

void trimSpaces(char *string) {
    if (*string == '\0') return;

    size_t length = strlen(string);
    char *start = string, *end = string + length;

    while (isspace(*start)) start += 1;

    if (end > start) {
        while (isspace(*--end)) *end = '\0';
    }

    if (start != string) {
        memmove(string, start, end - start + 1);
        memset(end, '\0', start - string);
    }
}

extern inline size_t stringConcat(char *dst, const char *src) {
    size_t written = 0;
    char *end = dst;
    while (*end) end += 1;

    while (*src) {
        *end++ = *src++;
        written += 1;
    }

    *end = '\0';
    return written;
}

#define ERR_INVALID_UTF8 "found invalid UTF-8"

/* Trims a 'long' (NUL terminated) UTF-8 encoded string if it exceeds
 * [cells] characters (codepoints) and inserts '...' at the beginning */
char *trimUTF8StringTo(const char *str, size_t cells) {
    if (cells <= 3) return GlobalArenaPushString("...");

    if (!str) return GlobalArenaPushString("(null)");

    /* TODO: port UCS width code to here so we can get a more accurate
     * estimated character width when calculating the trimmed length */
    size_t bufIdx = 0, chars = 0;
    size_t bufLen = strlen(str);

    /* we're supposed to treat the bytes as unsigned internally */
    unsigned char *buf = (unsigned char*)GlobalArenaPushString(str);

    /* walk backwards through the string's bytes
       until we reach the max amount of symbols */
    for (bufIdx = bufLen - 1; (bufIdx > 0) && (chars < cells - 3); chars++) {
        /* first check for ASCII bytes */
        if (!(buf[bufIdx] & 0x80)) {
            bufIdx -= 1;
            continue;
        } else if (bufIdx == 0) {
            return GlobalArenaPushString(ERR_INVALID_UTF8
                " (leading non-ASCII byte)");
        }

        if ((buf[bufIdx] & 0xC0) != 0x80) {
            return GlobalArenaPushString(ERR_INVALID_UTF8
                " (illegal continuation byte)");
        }

        bufIdx -= 1; // we have ourselves a continuation byte :DDDDD

        /* next 2 bytes should be a valid continuation or leading one
           and the 3rd has to be a leading byte in case we get there */
        for (int i = 0; i < 3; i++) {
            if (
                ((buf[bufIdx] & 0xE0) == 0xC0) || // 2-byte leading code unit
                ((buf[bufIdx] & 0xF0) == 0xE0) || // 3-byte leading code unit
                ((buf[bufIdx] & 0xF8) == 0xF0)    // 4-byte leading code unit
            ) {
                if (buf[bufIdx] <= 0xC1 || buf[bufIdx] >= 0xF5) {
                    return GlobalArenaPushString(ERR_INVALID_UTF8
                        "(illegal leading byte)");
                }

                bufIdx -= 1;
                chars += 1; // all 3-byte chars seem to occupy 2 spaces

                break;
            } else if (((buf[bufIdx] & 0xC0) == 0x80) && (i < 2)) {
                bufIdx -= 1;
            } else {
                return GlobalArenaPushString(ERR_INVALID_UTF8
                    " (continuation byte out of place)");
            }
        }
    }

    bufIdx += 1; // shift index to compensate for the last iteration's decrement

    if (bufIdx > 2) {
        for (size_t i = 0; i < 3; i++) {
            buf[--bufIdx] = '.';
        }

        size_t bytes = strlen((char*)buf + bufIdx) + 1;
        memmove(buf, buf + bufIdx, bytes);
        memset(buf + bytes - 1, 0, (bufLen + 1) - bytes);
    }

    return (char*)buf;
}

void readLine(char *dst, size_t dstSize) {
#ifdef _WIN32
    wchar_t wideBuf[ARG_BUF];

    if (!fgetws(wideBuf, ARG_BUF, stdin)) {
        printErr("unable to read from stdin", strerror(errno));
        exit(errno);
    }

    UTF16toUTF8(wideBuf, -1, dst, dstSize);
#else
    if (!fgets(dst, dstSize, stdin)) {
        printErr("unable to read from stdin", strerror(errno));
        exit(errno);
    }
#endif

    trimSpaces(dst);
}

extern inline bool isDirectory(const char *dir) {
#ifndef _WIN32
    struct stat pathStats;

    if (stat(dir, &pathStats) != 0) return false;

    return S_ISDIR(pathStats.st_mode);
#else
    wchar_t dirW[PATH_BUF];
    formatPathToWIN32(dir, dirW);
    DWORD fileAttr = GetFileAttributesW(dirW);

    if (fileAttr == INVALID_FILE_ATTRIBUTES) return false;

    return fileAttr & FILE_ATTRIBUTE_DIRECTORY;
#endif
}

extern inline bool isZeroMemory(const void *buf, const size_t bytes) {
    size_t chunks = bytes / sizeof(size_t);
    size_t remainder = bytes % sizeof(size_t);
    size_t i = 0;

    while (i < chunks) {
        if (((size_t*)buf)[i++]) return false;
    }

    if (remainder) {
        size_t chunkBytes = chunks * sizeof(size_t);
        i = chunkBytes;

        while (i < chunkBytes + remainder) {
            if (((char*)buf)[i++]) return false;
        }
    }

    return true;
}

extern inline size_t getNumberOfOnlineThreads(void) {
#ifdef _WIN32
    SYSTEM_INFO sysInfo = {0};
    GetSystemInfo(&sysInfo);
    return (size_t)sysInfo.dwNumberOfProcessors;
#else
    return  (size_t)sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

extern char *getAbsolutePath(const char *dir) {
#ifdef _WIN32
    wchar_t dirW[PATH_BUF];
    UTF8toUTF16(dir, -1, dirW, PATH_BUF);
    wchar_t absDirW[PATH_BUF];
    GetFullPathNameW(dirW, PATH_BUF, absDirW, NULL);
    DWORD sz = UTF16toUTF8(absDirW, -1, NULL, 0);
    char *absDir = GlobalArenaPush(sz * sizeof(char));
    UTF16toUTF8(absDirW, -1, absDir, sz);

    return absDir;
#else
    /* TODO:
     * - do UNIX realpath() stuff here and test it
     *   with very long paths (larger than PATH_MAX bytes)
     * NOTE:
     * - paths longer than PATH_MAX bytes will probably always break
     *   here since realpath() only allows up to that much space */

    char *realPath = GlobalArenaPush(PATH_MAX);

    if (!realpath(dir, realPath)) {
        printErr("unable to get resolved path", strerror(errno));
        return NULL;
    }

    return realPath;
#endif
}

extern char *getCurrentWorkingDirectory(void) {
    char *cwd = NULL;

#ifdef _WIN32
        int len = GetCurrentDirectoryW(0, NULL);
        wchar_t *currentDirW = GlobalArenaPush(len * sizeof(wchar_t));
        GetCurrentDirectoryW((DWORD)len, currentDirW);
        len = UTF16toUTF8(currentDirW, -1, NULL, 0);
        cwd = GlobalArenaPush(len * sizeof(char));
        UTF16toUTF8(currentDirW, -1, cwd, len);
#else
        cwd = GlobalArenaPush(PATH_MAX * sizeof(char));

        if (!getcwd(cwd, PATH_MAX)) {
            printErr(
                "unable to retrieve current working directory",
                strerror(errno)
            );

            return NULL;
        }
#endif

        return cwd;
}

extern inline void printVersionPage(const char *arg) {
    printf(" executable: " VERSION_ATTRIBUTES "\n");
    char *installDir = getAbsolutePath(arg);
    char *end = installDir + strlen(installDir) - 1;
    while (*--end != PATH_SEP);
    *end = '\0';

    printf(" installed @ %s\n", installDir);

#if defined __clang__
    printf(" built with: clang " __clang_version__ "\n");
#elif defined __GNUC__
    printf(" built with: gcc " __VERSION__ "\n");
#elif defined _MSC_VER
    printf(" built with: msvc %zu\n", (size_t)_MSC_VER);
#endif

    printf("\n");
}

#ifndef _WIN32
#define ATTR_STACK_SIZE (8 * 4096)

extern inline void threadAttrInit(pthread_attr_t *attr) {
    int err = 0;

    if ((err = pthread_attr_init(attr))) {
        printErr("unable to initialize thread attributes", strerror(err));
        exit(err);
    }

    if ((err = pthread_attr_setstacksize(attr, ATTR_STACK_SIZE))) {
        printErr("unable to set threads' stack size", strerror(err));
        exit(err);
    }
}

extern inline void threadAttrDestroy(pthread_attr_t *attr) {
    int err = 0;

    if ((err = pthread_attr_destroy(attr))) {
        printErr("unable to destroy thread attributes", strerror(err));
        exit(err);
    }
}
#endif
