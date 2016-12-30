#ifndef __CV4_B__
#define __CV4_B__

#include <sys/types.h>
#include <stdbool.h>

#define kCv4BMagic   {'C', 'A', 'R', '\0'}
#define kCv4BVersion {'4', '.', 'B'}

extern bool Cv4BArchiveCreate(const char *archive, const char *rootdir);
extern bool Cv4BArchiveListItems(const char *archive, const char *const *paths, size_t *listSize);
extern bool Cv4BArchiveExtractItem(const char *archive, const char *path, const char *file);
extern bool Cv4BArchiveExtractAllItems(const char *archive, const char *directory);
extern bool Cv4BArchiveBasicValidate(const char *archive, bool *isValid);
extern bool Cv4BArchiveStrictValidate(const char *archive, bool *isValid);
extern bool Cv4BArchiveSplit(const char *archive, const char *directory);
extern bool Cv4BArchiveMerge(const char *archive, const char *directory);

#endif /* __CV4_B__ */
