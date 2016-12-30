#ifndef __LIST__
#define __LIST__ 1

#include <sys/types.h>
#include <stdbool.h>

typedef enum {
    kFLEntryTypeDirectory = 0,
    kFLEntryTypeRegular   = 1,
    kFLEntryTypeSymlink   = 2
} FLEntryType;

typedef struct __FListEntry {
    struct __FListEntry *next, *prev;

    FLEntryType type;
    size_t size;
    char *name;
} FListEntry;

typedef struct {
    FListEntry *head, *tail;
    size_t size;
} FList;

extern FList *FListCreate(void);
extern bool FListAddDirectory(FList *list, const char *directory);
extern void FListAddEntry(FList *list, char *name, FLEntryType type, size_t size);
extern void FListDestroy(FList *list);

#endif /* !defined(__LIST__) */
