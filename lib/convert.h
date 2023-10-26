#ifndef HEADER_CONVERT
#define HEADER_CONVERT

#include <data.h>
#include <parse.h>

#define TV_NSEC_MAX (1e9 - 1)
#define TIMEOUT_MS 10

int convertFiles(const char **files, Arguments *args, ProcessInfo *stats);

#endif  /* HEADER_CONVERT */
