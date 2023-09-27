#include "data.h"
#include <search.h>

#define INITIAL_LIST_BUF  (8)
#define LINE_LEN          (80)

static char **_getFilesFromDir(const char *dir,
                               const char **fmts,
                               const bool recurse);

static int fileErr = 0;

char **getFiles(const arguments *args) {
    size_t listSize = INITIAL_LIST_BUF;
    char **list = xcalloc(listSize, sizeof(char*));
    size_t listIdx = 0;

    for (int i = 0; args->inPaths[i]; i++) {
        const char *dir = args->inPaths[i];
        char *trimmedDir = trimUTF8StringTo(dir, LINE_LEN - 40);

        if (!isDirectory(dir)) {
            printf("%s \"%s\"%s is not a directory (%signoring%s)\n\n",
                   COLOR_INPUT, trimmedDir, COLOR_DEFAULT,
                   COLOR_ACCENT, COLOR_DEFAULT);

            free(trimmedDir);
            continue;
        }

        printf("%s scanning %s@ %s\"%s\"%s\n\n",
               COLOR_DEFAULT, COLOR_ACCENT,
               COLOR_INPUT, trimmedDir, COLOR_DEFAULT);

        char **files = _getFilesFromDir(dir, (const char**)args->inFormats,
                                        !(args->options & OPT_NORECURSION));

        size_t fileCount = 0;

        if (fileErr) {
            free(trimmedDir);
            continue;
        }

        if (files)
            while (files[fileCount])
                fileCount++;

        printf("%s (found %s%lu%s files)%s\n\n",
               COLOR_DEFAULT, COLOR_ACCENT,
               (unsigned long)fileCount,
               COLOR_DEFAULT, COLOR_DEFAULT);

        free(trimmedDir);

        if (!files) {
            free(trimmedDir);
            continue;
        }

        for (int idx = 0; files[idx]; idx++) {
            if (listIdx == listSize - 1) {
                size_t newSize = listSize * 2;

                xrealloc(list, newSize * sizeof(char*));
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

static char **_getFilesFromDir(const char *dir,
                               const char **fmts,
                               const bool recurse) {
    size_t listSize = INITIAL_LIST_BUF;
    size_t listIdx = 0;
    char **list = xcalloc(listSize, sizeof(char*));

    fileErr = 0;

    DIR *d = opendir(dir);
    struct dirent *entry = NULL;

    if (!d) {
        fileErr = errno;
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
        char *fullInPath = _asprintf("%s%c%s", dir, PATH_SEP, fileName);

        if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0) {
            free(fileName);
            free(fullInPath);
            continue;
        }

        /* Recursively search in case it's a directory */
        if (isDirectory(fullInPath) && recurse) {
            char **recList = _getFilesFromDir(fullInPath, fmts, recurse);

            if (recList) {
                for (int i = 0; recList[i]; i++) {
                    if (listIdx == listSize - 1) {
                        size_t newSize = listSize * 2;

                        xrealloc(list, newSize * sizeof(char*));
                        memset((list + listSize), 0,
                               (newSize - listSize) * sizeof(char*));

                        listSize = newSize;
                    }

                    list[listIdx++] = strdup(recList[i]);
                    free(recList[i]);
                }

                free(recList);
            }

            free(fileName);
            free(fullInPath);
            continue;
        }

        for (int i = 0; fmts[i]; i++) {
            char ext[NAME_MAX] = "";
            snprintf(ext, NAME_MAX, ".%s", fmts[i]);

            const char *extPoint = strstr(fileName, ext);

            if (extPoint && extPoint[strlen(ext)] == '\0') {
                if (listIdx == listSize - 1) {
                    size_t newSize = listSize * 2;
                    xrealloc(list, newSize * sizeof(char*));
                    memset(list + listSize, 0,
                           (newSize - listSize) * sizeof(char*));

                    listSize = newSize;
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

    xrealloc(list, (listIdx + 1) * sizeof(char*));
    list[listIdx] = NULL;

    return list;
}
