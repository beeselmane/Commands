#ifndef __CAR_V1__
#define __CAR_V1__ 1

#define OFFSET_BASE 0
#define HEADER_SIZE 33
#define ENTRY_SIZE  18

#include "car-priv.h"
#include "car.h"

typedef UInt16 CRv1Flags;
typedef char Char;

struct __CRv1LLENT;

typedef struct __CRv1LLENT {
    CString value;
    Size fileSize;
    struct __CRv1LLENT *next;
} *CRv1LinkedListEntry;

typedef struct __CRv1LinkedList {
    CRv1LinkedListEntry first, last;
    Size length;
} *CRv1LinkedList;

typedef struct __CARHeaderV1 {
    Char magic[3];
    UInt8 version;
    UInt8 flags;
    Offset toc;
    UInt64 filecount;
    Size fullsize;
    Size extsize;
    UInt32 checksum;
} *CRv1CARHeaderRef;

typedef struct __CARFileEntryV1 {
    CString name;
    UInt8 type;
    Offset data;
    Size size;
    UInt16 checksum;
    UInt8 mdSize;
} *CRv1CARFileEntryRef;

struct __CARFileV1 {
    CRv1CARHeaderRef header;
    CRv1LinkedList fileList;
    CRv1CARFileEntryRef *toc;
};

bool CRv1CreateCAR(CString rootdir, CString car, CString *error, bool verbose, bool checksum);
CString *CRv1ListFiles(CString car, UInt64 *count, CRv1Flags flags);
bool CRv1GrabFile(CString car, CString file, CString out, CRv1Flags flags);
bool CRv1ExtractCar(CString car, CString outdir, CRv1Flags flags);

#endif /* __CAR_V1__ */
