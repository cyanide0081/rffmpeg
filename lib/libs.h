#ifndef H_LIBS
#define H_LIBS

/* OS headers */
#if defined _WIN32
#include <windows.h>
#include <shellapi.h>
#include <process.h>
#include <fcntl.h>
#include <io.h>
#elif defined __linux__ || defined __APPLE__ || defined __FreeBSD__
/* don't ask me why */
#if defined __FreeBSD__
#define __BSD_VISIBLE 1
#define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC_PRECISE
#elif defined __APPLE__
#define _DARWIN_C_SOURCE 1
#endif

#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/mman.h>

#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>

#endif /* __linux__ */

#else
#error "FATAL: unsupported OS/Kernel"
#endif /* OS headers */

/* CRT headers */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>

#endif /* H_LIBS */
