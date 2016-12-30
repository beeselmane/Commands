#ifndef __JAVA__
#define __JAVA__ 1

#include <sys/types.h>
#include <stdbool.h>

#define null 0

#define JVMEnum(type, name) typedef type name; enum

typedef uint8_t  UInt8;
typedef  int8_t  SInt8;
typedef uint16_t UInt16;
typedef  int16_t SInt16;
typedef uint32_t UInt32;
typedef  int32_t SInt32;
typedef uint64_t UInt64;
typedef  int64_t SInt64;

typedef unsigned __int128 UInt128;
typedef signed   __int128 SInt128;




typedef struct {
    UInt64 pc;
} JVMThreadData;

typedef struct {
    pthread_mutex_t *lock;
} JVMGlobalData;

#endif /* !defined(__JAVA__) */
