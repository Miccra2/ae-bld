// BLD is based on https://github.com/tsoding/nob.h
#ifndef __BLD_H__
#define __BLD_H__ 1

// used to declare functions `static` or `inline`
#ifndef BLD_DEF
#define BLD_DEF
#endif // BLD_DEF

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#ifndef BLD_ASSERT
#define BLD_ASSERT assert
#endif // BLD_MALLOC

#ifndef BLD_REALLOC
#define BLD_REALLOC realloc
#endif // BLD_REALLOC

#ifndef BLD_FREE
#define BLD_FREE free
#endif // BLD_FREE

#ifdef _WIN32
    #define NEWLINE "\r\n"
    #define _IMM_
    #define _WINCON_
    #define _WINGDI_
    #define _WINUSER_
    #define WIN32_LEAN_AND_MEAN
    #include <io.h>
    #include <direct.h>
    #include <windows.h>
    #include <shellapi.h>
#else  // _WIN32
    #define NEWLINE "\n"
    #include <fcntl.h>
    #include <dirent.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/wait.h>
    #include <sys/types.h>
#endif // _WIN32

#define BLD_FMT_ERROR(format, ...)   fprintf(stderr, "Error: "   format __VA_OPT__(,) __VA_ARGS__)
#define BLD_FMT_WARNING(format, ...) fprintf(stderr, "Warning: " format __VA_OPT__(,) __VA_ARGS__)
#define BLD_FMT_INFO(format, ...)    fprintf(stderr, "Info: "    format __VA_OPT__(,) __VA_ARGS__)

// BLD_DA_MIN_CAP should be a multiple of 4096 for page alignment
#ifndef BLD_DA_MIN_CAP
#define BLD_DA_MIN_CAP (4096)
#endif // BLD_DA_MIN_CAP

#define BLD_DA_DECL(type, name) struct _##name {    \
    size_t capacity;                                \
    size_t length;                                  \
    type *buffer;                                   \
} name, *p##name

#define BLD_DA_EMPTY        { .capacity=0, .length=0, .buffer=NULL, }
#define BLD_DA_FIRST(da)    (da).buffer[BLD_ASSERT((da).length > 0), 0]
#define BLD_DA_LAST(da)     (da).buffer[BLD_ASSERT((da).length > 0), (da).length - 1]
#define BLD_DA_GET(da, idx) (da).buffer[BLD_ASSERT((da).length > (idx)), (idx)]
#define BLD_DA_POP(da)      (da).buffer[BLD_ASSERT((da).length > 0), --(da).length]

#define BLD_DA_FREE(da) do {                        \
    BLD_FREE((da).buffer);                          \
    (da) = (typeof(da))BLD_DA_EMPTY;                \
} while (0)

#define BLD_DA_PUSH(da, it) do {                    \
    BLD_DA_ALLOCATE(da, (da).length + 1);           \
    (da).buffer[(da).length++] = (it);              \
} while (0)

#define BLD_DA_APPEND(da, it, len)  do {            \
    BLD_DA_ALLOCATE(da, (da).length + (len));       \
    for (size_t i = 0; i < (len); i++)              \
        (da).buffer[(da).length++] = (it)[i];       \
} while (0)

#define BLD_DA_ALLOCATE(da, len) do {               \
    if ((da).capacity < (len)) {                    \
        if ((da).cpacity == 0) {                    \
            (da).capacity = DA_MIN_CAP;             \
        }                                           \
        while ((da).capacity < (len)) {             \
            (da).capacity *= 2;                     \
        }                                           \
        (da).buffer = BLD_REALLOC(                  \
            (da).buffer,                            \
            (da).capacity * sizeof(*(da).buffer)    \
        );                                          \
    }                                               \
} while (0)

typedef struct {
    char *path;
    char *text;
    char *eof;
    FILE *pf;
} BLD_FileInfo;

BLD_DEF BLD_FileInfo bld_read_entire_file(const char *path);
BLD_DEF bool bld_text_ends(BLD_FileInfo *file_info, char *cursor, ssize_t offset);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __BLD_H__

#ifdef BLD_IMPL

BLD_DEF BLD_FileInfo bld_read_entire_file(const char *path) {
    FILE *pf = fopen(path, "r");
    if (!pf) {
        BLD_FMT_ERROR("%s: Could not open file!", path);
        exit(1);
    }
    fseek(pf, 0, SEEK_END);
    size_t size = ftell(pf);
    char *buffer = malloc(size);
    if (!buffer) {
        BLD_FMT_ERROR("%s: Could not allocate text buffer!", path);
        exit(1);
    }
    rewind(pf);
    fread(buffer, 1, size, pf);
    fclose(pf);
    return (BLD_FileInfo){ .path=(char*)path, .text=buffer, .eof=(buffer+size), .pf=NULL, };
}

BLD_DEF bool bld_text_ends(BLD_FileInfo *file_info, char *cursor, ssize_t offset) {
    return cursor + offset >= file_info->eof;
}

#endif // BLD_IMPL

#ifdef BLD_STRIP_PREFIX
#define FMT_ERROR        BLD_FMT_ERROR
#define FMT_WARNING      BLD_FMT_WARNING
#define FMT_INFO         BLD_FMT_INFO
#define DA_MIN_CAP       BLD_DA_MIN_CAP
#define DA_DECL          BLD_DA_DECL
#define DA_EMPTY         BLD_DA_EMPTY
#define DA_FIRST         BLD_DA_FIRST
#define DA_LAST          BLD_DA_LAST
#define DA_GET           BLD_DA_GET
#define DA_POP           BLD_DA_POP
#define DA_FREE          BLD_DA_FREE
#define DA_PUSH          BLD_DA_PUSH
#define DA_APPEND        BLD_DA_APPEND
#define DA_ALLOCATE      BLD_DA_ALLOCATE
#define FileInfo         BLD_FileInfo
#define read_entire_file bld_read_entire_file
#define text_ends        bld_text_ends
#endif // BLD_STRIP_PREFIX
