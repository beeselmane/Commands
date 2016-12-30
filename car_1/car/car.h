#ifndef __CAR__
#define __CAR__ 1

#include <sys/types.h>
#include <stdbool.h>

#define kCArchiveMagicSize    4
#define kCArchiveVersionSize  3
#define kCArchiveVersionCount 2

typedef enum {
    kCArchiveVersion_UNK = -1,
    kCArchiveVersion_4_B,
    kCArchiveVersionCurrent = kCArchiveVersion_4_B
} CArchiveVersion;

typedef struct {
    bool canCreate;
    bool canAddItems;
    bool canListItems;
    bool canExtractItems;
    bool canBasicValidate;
    bool canStrictValidate;
    bool canSplit;
    bool canMerge;
} CArchiveActions;

extern CArchiveActions CArchiveGetSupportedActionsForVersion(CArchiveVersion version);
extern CArchiveVersion CArchiveAttemptVersionDetectionForArchive(const char *archive);
extern bool CArchiveCreateWithVersion(CArchiveVersion version, const char *archive, const char *rootdir);
extern bool CArchiveAddItemWithVersion(CArchiveVersion version, const char *archive, const char *file, const char *path);
extern bool CArchiveListItemsWithVersion(CArchiveVersion version, const char *archive, const char *const *items, size_t *listSize);
extern bool CArchiveExtractItemWithVersion(CArchiveVersion version, const char *archive, const char *path, const char *file);
extern bool CArchiveExtractAllItemsWithVersion(CArchiveVersion version, const char *archive, const char *directory);
extern bool CArchiveBasicValidateWithVersion(CArchiveVersion version, const char *archive, bool *isValid);
extern bool CArchiveStrictValidateWithVersion(CArchiveVersion version, const char *archive, bool *isValid);
extern bool CArchiveSplitWithVersion(CArchiveVersion version, const char *archive, const char *directory);
extern bool CArchiveMergeWithVersion(CArchiveVersion version, const char *archive, const char *directory);

#endif /* !defined(__CAR__) */
