#ifndef unistring_h
#define unistring_h

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CXSwap16(x) ((uint16_t)             \
        (((uint16_t)(x) & 0xFF00) >> 8) |       \
        (((uint16_t)(x) & 0x00FF) << 8))

/*#define CXSwap32(x) ((uint32_t)             \
        (((uint32_t)(x) & 0xFF000000) >> 24) |  \
        (((uint32_t)(x) & 0x00FF0000) >>  8) |  \
        (((uint32_t)(x) & 0x0000FF00) <<  8) |  \
        (((uint32_t)(x) & 0x000000FF) << 24))*/

#define CXSwap32(x) __builtin_bswap32(x)

typedef enum CXEndian {
    kCXEndianLittle,
    kCXEndianBig
} CXEndian;

extern CXEndian CXEndianCurrent(void);

typedef uint64_t CXIndex;
typedef uint64_t CXSize;

typedef struct __CXString *CXStringRef;

typedef enum CXStringEncoding {
    kCXStringEncodingUnicode = (1 << 15),
    kCXStringEncodingUTF8    = 1 | kCXStringEncodingUnicode,
    kCXStringEncodingUTF16   = 2 | kCXStringEncodingUnicode,
    kCXStringEncodingUTF24   = 3 | kCXStringEncodingUnicode,
    kCXStringEncodingUTF32   = 4 | kCXStringEncodingUnicode,
} CXStringEncoding;

typedef uint32_t CXUnicodePoint;
typedef uint32_t CXUTF32Char;
typedef uint8_t  CXUTF24Char[3];
typedef uint16_t CXUTF16Char;
typedef uint8_t  CXUTF8Char;

extern CXStringRef CXStringCreateWithUTF8String(const uint8_t *bytes);
extern CXStringRef CXStringCreateWithBytes(const void *bytes, CXSize length, CXStringEncoding encoding, bool matchesNativeEndian);
extern CXSize CXStringGetLength(CXStringRef string);
extern CXStringEncoding CXStringGetSmallestEncoding(CXStringRef string);
extern uint64_t CXStringGetByteLengthWithEncoding(CXStringRef string, CXStringEncoding encoding);
extern CXUnicodePoint CXStringGetCharacterAtIndex(CXStringRef string, CXIndex index);
extern bool CXStringEqual(CXStringRef s1, CXStringRef s2);
extern CXSize CXStringCopyWithEncoding(CXStringRef string, void *buffer, CXSize bufferSize, CXSize *bufferUsed, CXStringEncoding encoding, bool includeBOM, CXEndian endian);

extern CXStringEncoding CXStringDefaultEncoding(void);

typedef struct __CXStringIterator *CXStringIteratorRef;

extern CXStringIteratorRef CXStringIteratorCreate(CXStringRef string);
extern CXStringIteratorRef CXStringIteratorCreateAtIndex(CXStringRef string, CXIndex index);
extern CXUnicodePoint CXStringIteratorNextCodePoint(CXStringIteratorRef iterator);
extern CXIndex CXStringIteratorGetNextIndex(CXStringIteratorRef iterator);
extern void CXStringIteratorReset(CXStringIteratorRef iterator);

extern void CXRetain(void *object);
extern void CXRelease(void *object);

#endif /* unistring_h */
