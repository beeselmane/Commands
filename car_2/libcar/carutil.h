#ifndef __CARUTIL__
#define __CARUTIL__ 1

#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <libcar.h>
#include <libgen.h>
#include <stdio.h>
#include <errno.h>

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// = CAR File List Interface                                     = //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

typedef struct {
    size_t fullsize;
    size_t namesize;
    size_t shift;
    size_t size;

    struct __CAREntry {
        void *data;
        struct __CAREntry *next;
    } *head, *tail;
} CARList;

typedef struct __CAREntry CAREntry;

CARList *CARListCreate();
CAREntry *CAREntryMake(void *data);
void CARListDestroy(CARList *list);

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// = String Interface                                            = //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

typedef struct {
    uint16_t length;
    char *string;
} CARString;

CARString *CARStringCreateWithCString(const char *cstring);
void CARStringDestroy(CARString *string);

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// = Endianess                                                   = //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

#ifdef BIG_ENDIAN
    #undef BIG_ENDIAN
#endif

#ifdef LITTLE_ENDIAN
    #undef LITTLE_ENDIAN
#endif

#ifdef __BIG_ENDIAN__
    #define BIG_ENDIAN    1
    #define LITTLE_ENDIAN 0
#endif

#ifdef __LITTLE_ENDIAN__
    #define BIG_ENDIAN    0
    #define LITTLE_ENDIAN 1
#endif

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// = Misc                                                        = //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

#define CARString2(v, s1, s2        ) asprintf(&v, "%s%s"    , s1, s2        )
#define CARString3(v, s1, s2, s3    ) asprintf(&v, "%s%s%s"  , s1, s2, s3    )
#define CARString4(v, s1, s2, s3, s4) asprintf(&v, "%s%s%s%s", s1, s2, s3, s4)

static void mkdirs(Path path)
{
    printf("Making %s...\n", path);
    char buffer[1024];
    char *tmp = NULL;

    strcpy(buffer, path);
    size_t length = strlen(buffer);

    if (buffer[length - 1] == '/') buffer[length - 1] = 0;

    for (tmp = buffer + 1; (*tmp); tmp++)
    {
        if ((*tmp) == '/')
        {
            (*tmp) = 0;
            mkdir(buffer, S_IRWXU | S_IRGRP | S_IROTH);
            (*tmp) = '/';
        }
    }

    mkdir(buffer, S_IRWXU | S_IRGRP | S_IROTH);
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// = Specific Entries                                            = //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

typedef struct {
    const char *filename;
    size_t filesize;
    uint16_t namesize;
} CV3Entry;

#endif /* !defined(__CARUTIL__) */
