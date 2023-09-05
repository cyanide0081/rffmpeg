#ifndef H_WIN
#define H_WIN

/* Implementations for functions that are only needed in Windows builds */
#ifdef _WIN32

#include "libs.h"
#include "types.h"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Substitutions for MSVC compiler */
#ifdef _MSC_VER

#include <basetsd.h>

typedef SSIZE_T ssize_t;

#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define strtok_r strtok_s
#define CLOCK_MONOTONIC_RAW 0

#endif

#define DIR _WDIR
#define dirent _wdirent
#define strdup _strdup
#define memccpy _memccpy

#define mkdir(a, b) mkdirWin(a, b)
#define opendir(d) opendirWin(d)
#define readdir(d) _wreaddir(d)
#define closedir(d) _wclosedir(d)
#define UTF8toUTF16(mbs, mbc, wcs, wcc) \
    MultiByteToWideChar(CP_UTF8, 0, mbs, mbc, wcs, wcc)
#define UTF16toUTF8(wcs, wcc, mbs, mbc) \
    WideCharToMultiByte(CP_UTF8, 0, wcs, wcc, mbs, mbc, NULL, NULL)

/* Implementation of clock_gettime for win32 */
int clock_gettime(int, struct timespec *spec);

/* Implementation of POSIX's getline() since it's unavailable in MSVCRT */
ssize_t getline(char **string, size_t *buffer, FILE *stream);

/* Overrides mkdir to get around differences between std and ms versions */
int mkdirWin(const char *dir, int mode);

/* Overrides opendir() to support UNICODE directories on Windows */
DIR *opendirWin(const char *dir);

#endif // _WIN32
#endif // H_WIN
