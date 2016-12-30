#ifndef __CAR_CT__
#define __CAR_CT__ 1

#include <stdint.h>

typedef uint8_t  UInt8;
typedef uint16_t UInt16;
typedef uint32_t UInt32;
typedef uint64_t UInt64;

typedef int8_t  SInt8;
typedef int16_t SInt16;
typedef int32_t SInt32;
typedef int64_t SInt64;

typedef void *MemoryAddress;
typedef char *CString;
typedef UInt64 Offset;
typedef UInt64 Size;

typedef struct __FileList *FileList;
struct __FLENT;

struct __CARFileV3 {
    char magic[3];
    UInt8 version;
    UInt16 flags;
    Offset toc;
    Offset data;
    UInt64 filecount;
    UInt16 namesize;
    Size fullsize;
    Size extsize;
    UInt32 hdrCheck;
    Offset signature;
    FileList files;
    bool transactionActive;
    bool transactionModified;
    FILE *fp;
};

typedef struct __FLENT {
    CString name;
    UInt32 check;
    Size namelen;
    UInt8 type;
    Size size;
    
    union {
        Offset dataOffset;
        FILE *fp;
    } dataRef;
    
    MemoryAddress metadata;
    Size metadataSize;
    
    struct __FLENT *next;
} *FileListEntry;

struct __FileList {
    FileListEntry first, last;
    Size maxNameLength;
    Size namesLength;
    Size length;
};

#endif /* __CAR_CT__ */
