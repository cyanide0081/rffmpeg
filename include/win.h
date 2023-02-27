#ifndef H_WIN
#define H_WIN

/* Implementations for functions that are only needed in Windows builds */
#ifdef _WIN32

#include "libs.h"
#include "types.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define DIR _WDIR
#define dirent _wdirent

#define mkdir(a, b) mkdirWin(a, b)
#define opendir(d) opendirWin(d)
#define readdir(d) _wreaddir(d)
#define closedir(d) _wclosedir(d)

/* Manual implementation of POSIX's getline() since it's unavailable in MSVCRT */
ssize_t getline(char **string, size_t *buffer, FILE *stream);

/* Overrides standard mkdir to get around differences between std and ms versions */
int mkdirWin(const char *dir, int mode);

/* Overrides opendir() to support UNICODE directories on Windows */
DIR *opendirWin(const char *dir);

/* Overrides readdir() to support UNICODE directories on Windows */
// struct dirent *readdirWin(DIR *dir);

#endif // _WIN32
#endif // H_WIN