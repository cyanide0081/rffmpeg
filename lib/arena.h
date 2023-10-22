#ifndef H_ARENA
#define H_ARENA

/* [code for a simple (growable) arena allocator]
 *
 * read deez below for more on allocation methods:
 *  -> https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator
 *  -> https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
 */

#include <libs.h>

/* NOTE: 64KB seemed pretty generous after some testing +
 * it's also the default allocation granularity on Windows */
#define ARENA_INIT_SIZE     (64 * 1024)
#define ARENA_GROWTH_FACTOR 2
#define ARENA_MEM_ALIGNMENT sizeof(void*)

typedef struct Arena {
    char *buf;          // actual arena memory
    size_t size;        // size of the buffer in bytes
    size_t pos;         // offset to first byte of (unaligned) free memory
    struct Arena *next; // pointer to next arena (for growable functionality)
} Arena;

extern Arena *globalArena;

#define GlobalArenaInit() {                     \
    globalArena = ArenaAlloc(ARENA_INIT_SIZE);  \
} (void)0

#define GlobalArenaRelease() {          \
    GlobalArenaPrintUsage();            \
    ArenaRelease(globalArena);          \
} (void)0

#define GlobalArenaPush(sz) ArenaPush(globalArena, sz)
#define GlobalArenaPushString(str) ArenaPushString(globalArena, str)
#define GlobalArenaSprintf(f, ...) ArenaSprintf(globalArena, f, __VA_ARGS__)
#define GlobalArenaPushStringN(str, n) ArenaPushStringN(globalArena, str, n)

static inline uintptr_t alignForward(uintptr_t ptr, size_t alignment) {
    assert((alignment & (alignment - 1)) == 0); // is power of 2

    uintptr_t p = ptr, a = (uintptr_t)alignment;
    uintptr_t mod = p & (a - 1);

    if (mod) p += a - mod;

    return p;
}

static inline Arena *ArenaAlloc(size_t bytes) {
#ifdef _WIN32
    Arena *arena = VirtualAlloc(
        NULL, sizeof(*arena), MEM_COMMIT, PAGE_READWRITE
    );
    arena->buf = VirtualAlloc(
        NULL, bytes, MEM_COMMIT, PAGE_READWRITE
    );
#else
    Arena *arena = sbrk(sizeof(*arena));
    arena->buf   = mmap(
        NULL, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0
    );
#endif

    if (!arena || !arena->buf) {
        fprintf(stderr, "FATAL: unable to allocate %zuB\n", bytes);
        exit(errno);
    }

    /* NOTE: last two assignments are very likely redundant since both
     * allocation procedures zero the memory before handing it to us */
    arena->size = bytes;
    arena->pos  = 0;
    arena->next = NULL;

    return arena;
}

static inline void *ArenaPush(Arena *arena, size_t bytes) {
    arena->pos = alignForward(arena->pos, ARENA_MEM_ALIGNMENT);

    if (arena->pos + bytes > arena->size) {
        /* need more memory! (make growable arena linked list) */
        if (bytes > arena->size) {
            fprintf(
                stderr, " FATAL: requested block is too big for one arena! "
                "(Bl: %zuB, Ar: %zuB)\n", bytes, arena->size
            );
            exit(-1);
        }

        if (!arena->next) {
            size_t newSize = arena->size * ARENA_GROWTH_FACTOR;
#ifdef INSTRUMENTATION
            printf(" ALLOCATING: new arena of %zuB...\n\n", newSize);
#endif
            arena->next = ArenaAlloc(newSize);
            return ArenaPush(arena->next, bytes);
        }

        return ArenaPush(arena->next, bytes);
    }

    size_t pos  = arena->pos;
    arena->pos += bytes;

    return (void*)(arena->buf + pos);
}

static inline char *ArenaPushString(Arena *arena, const char *string) {
    size_t bytes = 0;
    while (*(string + bytes++));

    char *buf = ArenaPush(arena, bytes);

    size_t idx = 0;
    while((*(buf + idx) = *(string + idx))) idx++;

    return buf;
}

static inline void GlobalArenaPrintUsage(void) {
#ifdef INSTRUMENTATION
    Arena *a = globalArena;
    size_t mem = a->size, usage = a->pos;

    while ((a = a->next)) mem += a->size, usage += a->pos;

    printf(
        " total arena memory used: %zu%% of %.2fKB\n\n",
        100 * usage / mem, mem / 1024.0F
    );
#endif
}

static inline void ArenaRelease(Arena *arena) {
    if (arena->next) {
        ArenaRelease(arena->next);
    }

#ifdef LOG
    printf(" RELEASING: arena @ 0x%p...\n\n", (void*)arena->buf);
    GlobalArenaPrintUsage();
#endif

#ifdef _WIN32
    VirtualFree(arena->buf, 0, MEM_RELEASE);
    VirtualFree(arena, 0, MEM_RELEASE);
#else
    munmap(arena->buf, arena->size);
#endif
}

static inline void __attribute__((unused)) ArenaFlush(Arena *arena) {
    if (arena->next) {
        ArenaRelease(arena->next);
    }

    memset(arena->buf, 0, arena->size);
    arena->pos  = 0;
    arena->next = NULL;
}

static inline char *ArenaSprintf(Arena *arena, const char *format, ...) {
    va_list args;
    va_start(args, format);

    size_t bytes = vsnprintf(NULL, 0 , format, args) + 1;
    char *s = ArenaPush(arena, bytes * sizeof(char));

    va_end(args);
    va_start(args, format);
    vsprintf(s, format, args);
    va_end(args);

    return s;
}

static inline char *ArenaPushStringN(Arena *arena, const char *str, size_t n) {
    if (strlen(str) <= n) {
        return ArenaPushString(arena, str);
    }

    char *s = ArenaPush(arena, (n + 1) * sizeof(char));
    return memcpy(s, str, n);
}

#endif /* H_ARENA */
