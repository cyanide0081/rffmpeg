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
    if (*string == '\0')
        return;

    size_t length = strlen(string);
    char *start = string;

    while (isspace(*start))
        start++;

    char *end = string + length;

    if (end > start)
        while (isspace(*--end))
            *end = '\0';

    if (start != string) {
        memmove(string, start, end - start + 1);
        memset(end, '\0', start - string);
    }
}

#define ERR_INVALID_UTF8 "found invalid UTF-8"

/* Trims a long (NUL terminated) UTF-8 encoded string if it exceeds
[maxChars] characters (codepoints) and inserts '...' at the start */
char *trimUTF8StringTo(const char *str, size_t maxChars) {
    if (maxChars <= 3)
        return GlobalArenaPushString("...");
    if (!str)
        return GlobalArenaPushString("(null)");

    /* TODO: (1) check for overlong encodings and stuff aswell
       (2) actually count glyphs inside the loop instead of just
       codepoints, since one glyph may be made out of more than
       one codepoint
       NOTE: '(2)' is probably impossible to do without getting info
       from the renderer itself since the length of a glyph will
       depend on the symbol font it's being rendered with (;-;) */

    size_t bufIdx = 0, chars = 0;
    size_t bufLen = strlen(str);

    /* we're supposed to treat the bytes as unsigned internally */
    unsigned char *buf = (unsigned char*)GlobalArenaPushString(str);

    /* walk backwards through the string's bytes
       until we reach the max amount of symbols */
    for (bufIdx = bufLen - 1; (bufIdx > 0) && (chars < maxChars - 3); chars++) {
        /* first check for ASCII bytes */
        if (!(buf[bufIdx] & 0x80)) {
            bufIdx--;
            continue;
        } else if (bufIdx == 0) {
            return GlobalArenaPushString(
                ERR_INVALID_UTF8 " (leading non-ASCII byte)"
            );
        }

        if ((buf[bufIdx] & 0xC0) != 0x80) {
            return GlobalArenaPushString(
                ERR_INVALID_UTF8 " (illegal continuation byte)"
            );
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
                    return GlobalArenaPushString(
                        ERR_INVALID_UTF8
                        "(illegal leading byte)"
                    );
                }

                bufIdx--, chars++; // all 3-byte chars seem to occupy 2 spaces
                break;
            } else if (((buf[bufIdx] & 0xC0) == 0x80) && (i < 2)) {
                bufIdx--;
            } else {
                return GlobalArenaPushString(
                    ERR_INVALID_UTF8
                    " (continuation byte out of place)"
                );
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

bool isDirectory(const char *dir) {
#ifndef _WIN32
    struct stat pathStats;

    if (stat(dir, &pathStats) != 0)
        return false;

    return S_ISDIR(pathStats.st_mode);
#else
    wchar_t dirW[PATH_BUF];
    formatPathToWIN32(dir, dirW);

    DWORD fileAttr = GetFileAttributesW(dirW);

    if (fileAttr == INVALID_FILE_ATTRIBUTES)
        return false;
    if (fileAttr & FILE_ATTRIBUTE_DIRECTORY)
        return true;

    return false;
#endif
}

bool isZeroMemory(const void *buf, const size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (((char*)buf)[i]) return false;
    }

    return true;
}
