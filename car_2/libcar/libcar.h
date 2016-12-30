#ifndef __LIBCAR__
#define __LIBCAR__ 1

#include <stdbool.h>
#include <stdint.h>

#define ERR_NONE    ( 0)
#define ERR_ARCHIVE (-1)
#define ERR_ACCESS  (-2)
#define ERR_STAT    (-3)
#define ERR_LSTAT   ERR_STAT
#define ERR_OPEN    (-4)
#define ERR_MMAP    (-5)
#define ERR_OPENDIR (-6)

// Note: I hate const
#define const

typedef struct {
    void *address;
    size_t size;
} MemoryBlock;

typedef struct {
    const char *name;
    void *address;
    size_t size;

    MemoryBlock *base;
    void (^free)(void *self);
} FileBuffer;

typedef uint8_t Version[3];
typedef const char *Path;

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// = General CAR Interface                                       = //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

typedef struct __CARFile *CARFileRef;

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// = CAR Version 3 Interface                                     = //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

#define CAR_CIGAM_v3   '\0RAC'
#define CAR_MAGIC_v3   'CAR\0'
#define CAR_VERSION_v3 {'3', '.', 'A'}

typedef enum {
    kCV3IsBigEndian = (1 << 0),
    kCV3UTF16       = (1 << 1),
    kCV3UTF32       = (1 << 2),
    kCV3HasChecksum = (1 << 3)
} CARHeaderFlagsVersion3;

typedef struct {
    uint32_t magic;       // 'CAR' + a Null Byte
    Version  version;     // Set to 3.A
    uint8_t  flags;       // File Parsing Flags
    uint64_t data;        // Data file offset
    uint64_t filecount;   // Number of Archived Files
    uint64_t extsize;     // Extracted Size
    uint64_t archsize;    // Archive Size
    uint32_t checksum;    // Archive Checksum
    uint8_t  reserved[8]; // Fills out to 48 Bytes
} CARHeaderVersion3 __attribute__((aligned(8)));

typedef struct {
    uint64_t size;     // Length of file data
    uint64_t offset;   // Offset in the data section
} CAREntryVersion3 __attribute__((aligned(8)));

int CV3CreateArchive(Path archive, Path root, uint8_t flags);
int CV3ExtractArchive(Path archive, Path outdir);
FileBuffer *CV3GetFile(Path archive, Path file, int *error);
const char **CV3ListFiles(Path archive, size_t *count, int *error);

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// = CAR Version 4.A Interface                                   = //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// = CAR Version 4.B Interface                                   = //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// = CAR Version 4.C Interface                                   = //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

#endif /* !defined(__LIBCAR__) */
