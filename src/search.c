#include <search.h>

#define INITIAL_LIST_BUF 8
#define MAX_DIR_PRINT_LEN (80 - 20)

static bool _isDirectory(const char *dir);

static char **_getFilesFromDir(const char *dir,
                               const char **fmts,
                               const bool recurse);

char **getFiles(const arguments *args) {
    size_t listSize = INITIAL_LIST_BUF;
    char **list = xcalloc(listSize, sizeof(char*));
    size_t listIdx = 0;

    for (int i = 0; args->inPaths[i]; i++) {
        const char *dir = args->inPaths[i];
        char *trimmedDir = trimUTF8StringTo(dir, MAX_DIR_PRINT_LEN);

        printf("%s searching %s\"%s\"\n\n",
               CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD, trimmedDir);

        char **files = _getFilesFromDir(dir, (const char**)args->inFormats,
                                        !(args->options & OPT_NORECURSION));

        if (!files)
            continue;

        size_t fileCount = 0;

        while (files[fileCount])
            fileCount++;

        printf("%s found %s%02lu%s files in %s\"%s\"\n",
               CHARCOLOR_WHITE, CHARCOLOR_RED, (unsigned long)fileCount,
               CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD, trimmedDir);

        free(trimmedDir);

        for (int idx = 0; files[idx]; idx++) {
            if (listIdx == listSize - 1) {
                size_t newSize = listSize * 2;

                list = realloc(list, newSize * sizeof(char*));
                memset((list + listSize), 0,
                       (newSize - listSize) * sizeof(char*));

                listSize = newSize;
            }

            list[listIdx++] = strdup(files[idx]);
            free(files[idx]);
        }

        free(files);
    }

    if (!(*list)) {
        free(list);
        return NULL;
    }

    return list;
}

static char **_getFilesFromDir(const char *dir, const char **fmts, const bool recurse) {
    size_t listSize = INITIAL_LIST_BUF;
    size_t listIdx = 0;
    char **list = xcalloc(listSize, sizeof(char*));

    DIR *d = opendir(dir);
    struct dirent *entry = NULL;

    if (!d) {
        char *err = _asprintf("%s: %s", dir, strerror(errno));
        printErr("unable to open directory", err);

        free(err);
        free(list);

        return NULL;
    }

    while ((entry = readdir(d))) {
#ifdef _WIN32
        int size = UTF16toUTF8(entry->d_name, -1, NULL, 0);
        char *fileName = xcalloc(size, sizeof(char));
        UTF16toUTF8(entry->d_name, -1, fileName, size);
#else
        char *fileName = strdup(entry->d_name);
#endif
        char *fullInPath = _asprintf("%s/%s", dir, fileName);

        if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0) {
            free(fileName);
            free(fullInPath);
            continue;
        }

        /* Recursively search in case it's a directory */
        if (_isDirectory(fullInPath) && recurse) {
            char **recList = _getFilesFromDir(fullInPath, fmts, recurse);

            if (recList) {
                for (int i = 0; recList[i]; i++) {
                    if (listIdx == listSize - 1) {
                        size_t newSize = listSize * 2;

                        list = realloc(list, newSize * sizeof(char*));
                        memset((list + listSize), 0,
                               (newSize - listSize) * sizeof(char*));

                        listSize = newSize;
                    }

                    list[listIdx++] = strdup(recList[i]);
                    free(recList[i]);
                }

                free(recList);
            }

            /* dprintf("%sno files in: %s%s\n", */
            /*         CHARCOLOR_RED, */
            /*         CHARCOLOR_WHITE_BOLD, */
            /*         fullInPath); */

            free(fileName);
            free(fullInPath);
            continue;
        }

        for (int i = 0; fmts[i]; i++) {
            if (strstr(fileName, fmts[i])) {
                if (listIdx == listSize - 1) {
                    size_t newSize = listSize * 2;
                    list = realloc(list, newSize * sizeof(char*));
                    memset(list + listSize, 0,
                           (newSize - listSize) * sizeof(char*));
                }

                list[listIdx++] = strdup(fullInPath);
                break;
            }
        }

        free(fileName);
        free(fullInPath);
    }

    closedir(d);

    if (!(*list)) {
        free(list);
        return NULL;
    }

    list = realloc(list, (listIdx + 1) * sizeof(char*));
    list[listIdx] = NULL;

    return list;
}

static bool _isDirectory(const char *dir) {
#ifndef _WIN32
    struct stat pathStats;
    if (stat(dir, &pathStats) != 0)
        return false;

    return S_ISDIR(pathStats.st_mode);
#else
    wchar_t dirW[PATH_BUFFER];
    UTF8toUTF16(dir, -1, dirW, PATH_BUFFER);

    DWORD fileAttr = GetFileAttributesW(dirW);

    if (fileAttr == INVALID_FILE_ATTRIBUTES)
        return false;
    if (fileAttr & FILE_ATTRIBUTE_DIRECTORY)
        return true;

    return false;
#endif
}
