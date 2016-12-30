#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "car.h"
#include "Cv4B.h"

// For use in unsupported fields in CArchiveFunctions
bool __UNSUPPORTED(void) { return false; }

typedef struct {
    const uint8_t magicString[kCArchiveMagicSize];
    const uint8_t versionString[kCArchiveVersionSize];

    CArchiveVersion version;
} CArchiveVersionInternal;

typedef struct {
    uint8_t magic[kCArchiveMagicSize];
    uint8_t version[kCArchiveVersionSize];
} CArchiveSharedHeader;

typedef struct {
    void *CArchiveCreate;           // bool (*)(const char *archive, const char *rootdir)
    void *CArchiveAddItem;          // bool (*)(const char *archive, const char *file, const char *path)
    void *CArchiveListItems;        // bool (*)(const char *archive, const char *const *items, size_t *listSize)
    void *CArchiveExtractItem;      // bool (*)(const char *archive, const char *path, const char *file)
    void *CArchiveExtractAllItems;  // bool (*)(const char *archive, const char *directory)
    void *CArchiveBasicValidate;    // bool (*)(const char *archive, bool *isValid)
    void *CArchiveStrictValidate;   // bool (*)(const char *archive, bool *isValid)
    void *CArchiveSplit;            // bool (*)(const char *archive, const char *directory)
    void *CArchiveMerge;            // bool (*)(const char *merge, const char *directory)
} CArchiveFunctions;

static CArchiveFunctions CArchiveFunctions_UNK = {
    .CArchiveCreate           = __UNSUPPORTED,
    .CArchiveAddItem          = __UNSUPPORTED,
    .CArchiveListItems        = __UNSUPPORTED,
    .CArchiveExtractItem      = __UNSUPPORTED,
    .CArchiveExtractAllItems  = __UNSUPPORTED,
    .CArchiveBasicValidate    = __UNSUPPORTED,
    .CArchiveStrictValidate   = __UNSUPPORTED,
    .CArchiveSplit            = __UNSUPPORTED,
    .CArchiveMerge            = __UNSUPPORTED
};

static CArchiveFunctions CArchiveFunctions_4_B = {
    .CArchiveCreate           = Cv4BArchiveCreate,
    .CArchiveAddItem          = __UNSUPPORTED,
    .CArchiveListItems        = Cv4BArchiveListItems,
    .CArchiveExtractItem      = Cv4BArchiveExtractItem,
    .CArchiveExtractAllItems  = Cv4BArchiveExtractAllItems,
    .CArchiveBasicValidate    = Cv4BArchiveBasicValidate,
    .CArchiveStrictValidate   = Cv4BArchiveStrictValidate,
    .CArchiveSplit            = Cv4BArchiveSplit,
    .CArchiveMerge            = Cv4BArchiveMerge
};

static CArchiveActions CArchiveActions_UNK = {
    .canCreate              = false,
    .canAddItems            = false,
    .canListItems           = false,
    .canExtractItems        = false,
    .canBasicValidate       = false,
    .canStrictValidate      = false,
    .canSplit               = false,
    .canMerge               = false
};

static CArchiveActions CArchiveActions_4_B = {
    .canCreate              = true,
    .canAddItems            = false,
    .canListItems           = true,
    .canExtractItems        = true,
    .canBasicValidate       = true,
    .canStrictValidate      = true,
    .canSplit               = true,
    .canMerge               = true
};

static CArchiveVersionInternal CArchiveVersions[kCArchiveVersionCount] = {
    {
        .magicString = {'C', 'A', 'R', '\0'},
        .versionString = {'?', '.', '?'},
        .version = kCArchiveVersion_UNK
    }, {
        .magicString = kCv4BMagic,
        .versionString = kCv4BVersion,
        .version = kCArchiveVersion_4_B
    }
};

CArchiveActions CArchiveGetSupportedActionsForVersion(CArchiveVersion version)
{
    switch (version)
    {
        case kCArchiveVersion_UNK: return CArchiveActions_UNK;
        case kCArchiveVersion_4_B: return CArchiveActions_4_B;
    }
}

CArchiveFunctions CArchiveResolveFunctionsWithVersion(CArchiveVersion version)
{
    switch (version)
    {
        case kCArchiveVersion_UNK: return CArchiveFunctions_UNK;
        case kCArchiveVersion_4_B: return CArchiveFunctions_4_B;
    }
}

CArchiveVersion CArchiveAttemptVersionDetectionForArchive(const char *archive)
{
    int fd = open(archive, O_RDONLY);
    if (fd < 0) return kCArchiveVersion_UNK;

    CArchiveSharedHeader header = {0, 0};
    ssize_t bytesread = read(fd, &header, sizeof(CArchiveSharedHeader));
    close(fd);

    if (bytesread != sizeof(CArchiveSharedHeader))
        return kCArchiveVersion_UNK;

    for (int i = 0; i < kCArchiveVersionCount; i++)
    {
        CArchiveVersionInternal possibleVersion = CArchiveVersions[i];

        if (memcmp(header.magic, possibleVersion.magicString, kCArchiveMagicSize)) continue;
        if (memcmp(header.version, possibleVersion.versionString, kCArchiveVersionSize)) continue;

        return possibleVersion.version;
    }

    return kCArchiveVersion_UNK;
}

bool CArchiveCreateWithVersion(CArchiveVersion version, const char *archive, const char *rootdir)
{
    void *func = CArchiveResolveFunctionsWithVersion(version).CArchiveCreate;
    return ((bool (*)())func)(archive, rootdir);
}

bool CArchiveAddItemWithVersion(CArchiveVersion version, const char *archive, const char *file, const char *path)
{
    void *func = CArchiveResolveFunctionsWithVersion(version).CArchiveAddItem;
    return ((bool (*)())func)(archive, file, path);
}

bool CArchiveListItemsWithVersion(CArchiveVersion version, const char *archive, const char *const *items, size_t *listSize)
{
    void *func = CArchiveResolveFunctionsWithVersion(version).CArchiveListItems;
    return ((bool (*)())func)(archive, items, listSize);
}

bool CArchiveExtractItemWithVersion(CArchiveVersion version, const char *archive, const char *path, const char *file)
{
    void *func = CArchiveResolveFunctionsWithVersion(version).CArchiveExtractItem;
    return ((bool (*)())func)(archive, path, file);
}

bool CArchiveExtractAllItemsWithVersion(CArchiveVersion version, const char *archive, const char *directory)
{
    void *func = CArchiveResolveFunctionsWithVersion(version).CArchiveExtractAllItems;
    return ((bool (*)())func)(archive, directory);
}

bool CArchiveBasicValidateWithVersion(CArchiveVersion version, const char *archive, bool *isValid)
{
    void *func = CArchiveResolveFunctionsWithVersion(version).CArchiveBasicValidate;
    return ((bool (*)())func)(archive, isValid);
}

bool CArchiveStrictValidateWithVersion(CArchiveVersion version, const char *archive, bool *isValid)
{
    void *func = CArchiveResolveFunctionsWithVersion(version).CArchiveStrictValidate;
    return ((bool (*)())func)(archive, isValid);
}

bool CArchiveSplitWithVersion(CArchiveVersion version, const char *archive, const char *directory)
{
    void *func = CArchiveResolveFunctionsWithVersion(version).CArchiveSplit;
    return ((bool (*)())func)(archive, directory);
}

bool CArchiveMergeWithVersion(CArchiveVersion version, const char *archive, const char *directory)
{
    void *func = CArchiveResolveFunctionsWithVersion(version).CArchiveMerge;
    return ((bool (*)())func)(archive, directory);
}
