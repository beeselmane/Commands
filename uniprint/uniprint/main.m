#include <Foundation/Foundation.h>
#include "unistring.h"
#include <sys/stat.h>

#define CUSTOM   0
#define CFSTRING 1
#define CXSTRING 2

#define METHOD CFSTRING

UInt8 *CFStringGetUTF8String(CFStringRef string, CFIndex *size)
{
    CFRetain(string);
    CFIndex length = CFStringGetLength(string);
    CFIndex bufferLength = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
    UInt8 *buffer = malloc(bufferLength);
    CFIndex usedBufferSize = -1;

    CFIndex converted = CFStringGetBytes(string, CFRangeMake(0, length), kCFStringEncodingUTF8, 0xFE, false, buffer, bufferLength, &usedBufferSize);

    if (converted != length)
    {
        fprintf(stderr, "Error Converting CFStringRef to UTF-8 String!\n");
        return NULL;
    }

    if (size) (*size) = usedBufferSize;

    if (usedBufferSize < (bufferLength - 1))
    {
        UInt8 *result = malloc(usedBufferSize + 1);
        strncpy((char *)result, (const char *)buffer, usedBufferSize);
        result[usedBufferSize] = 0;
        free(buffer);

        return result;
    }

    buffer[usedBufferSize] = 0;
    CFRelease(string);
    return buffer;
}

UInt16 *CFStringGetUTF16String(CFStringRef string, CFIndex *size)
{
    CFRetain(string);
    CXIndex length = CFStringGetLength(string);
    CFIndex bufferLength = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF16LE);
    UInt16 *buffer = malloc(bufferLength);
    CFIndex usedBufferSize = -1;

    CFIndex converted = CFStringGetBytes(string, CFRangeMake(0, length), kCFStringEncodingUTF16LE, 0xFE, false, (UInt8 *)buffer, bufferLength, &usedBufferSize);

    if (converted != length)
    {
        fprintf(stderr, "Error Converting CFStringRef to UTF-16 String!\n");
        return NULL;
    }

    if (size) (*size) = usedBufferSize;

    if (usedBufferSize < bufferLength)
    {
        UInt16 *result = malloc(usedBufferSize);
        strncpy((char *)result, (const char *)buffer, usedBufferSize);
        free(buffer);

        return result;
    }

    CFRelease(string);
    return buffer;
}

struct __CXString {
    int refcount;
    void *destroy;
    
    CXStringEncoding internalEncoding;
    CXSize bufferSize;
    CXSize length;
    void *buffer;
    
    CXStringEncoding smallestEncoding;
    CXStringIteratorRef iterator;
};

int main(int argc, const char *const *argv)
{
#if 0
    const char **args = (const char **)(argv + 1);
    int count = argc - 1;

    uint32_t *points = malloc(count * sizeof(uint32_t));

    for (int i = 0; i < count; i++)
    {
        char *uname = NULL;
        asprintf(&uname, "0x%s", args[i]);

        char *error = NULL;
        points[i] = (uint32_t)strtol(uname, &error, 0x10);
        points[i] &= 0x10FFFF;
        free(uname);

        if (error && (*error) != '\0')
        {
            fprintf(stderr, "Error: '%s' is not a valid codepoint\n", args[i]);
            exit(EXIT_FAILURE);
        }
    }
#else
    //CXUnicodePoint points[9] = {0x12039, 0x4E00, 0x12421, 0x332E, 0x2B768, 0x6C82, 0x5E7A, 0x0021, 0x0021};
    //int count = 9;
#endif

#if 1
    struct stat stats;
    stat("/home/developer/home/strtest", &stats);
    char *s = malloc(stats.st_size);
    FILE *fp = fopen("/home/developer/home/strtest", "rb");
    fread(s, 1, stats.st_size, fp);
    fclose(fp);
#else
    //char *s = "𫞔";
    //char *s = "好, how a𒁱𒁱𒁱𒁱𒁱𒁱re you 我不𒁱𒁱𒁱𒁱知道。害得ⲩengl𒁮	𒁮	𒁮	𒁮	𒁮\nr	𓀂	𓀃	𓀄	𓀅	𓀆	𓀇	𓀈	𓀉	𓀊	𓀋	𓀌	𓀍	𓀎	𓀏𓀀	𓀁	𓀂	𓀃	𓀄	𓀅	𓀆	𓀇	𓀈	𓀉	𓀊	𓀋	𓀌	𓀍	𓀎	𓀏drxdtfcghjlnk;𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	more我 iuv 白醋和八部𒁫doing? ΪΫάέήίΰϊ的撒下ⲧϋόύώ\nf\n您好, how a𒁱𒁱𒁱𒁱𒁱𒁱re you 我不𒁱𒁱𒁱𒁱知道。害得ⲩengl𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒂄𒂄ish p𒂄𒂄lz𒁫t𒂄𓀀	𓀁	𓀂	𓀃	𓀄	𓀅	𓀆	𓀇	𓀈	𓀉	𓀊	𓀋	𓀌	𓀍	𓀎	𓀏𓀀	𓀁	𓀂	𓀃	𓀄	𓀅	𓀆	𓀇	𓀈	𓀉	𓀊	𓀋	𓀌	𓀍	𓀎	𓀏𓀀	𓀁	𓀂	𓀃	𓀄	𓀅	𓀆	𓀇	𓀈	𓀉	𓀊𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔		𓀋	𓀌	𓀍	𓀎	𓀏𓀀	π	ρ	ς	σ	τ	υ	φ	χ	ψ	ω	ϊ	ϋ	ό	ύ	ώ	Ϗ𓀁	𓀂	𓀃	𓀄	𓀅	𓀆	𓀇	𓀈	𓀉	𓀊	𓀋	𓀌	𓀍	𓀎	𓀏𓀀	𓀁	𓀂	𓀃	𓀄	𓀅	𓀆	𓀇	𓀈	𓀉	𓀊	𓀋	𓀌	𓀍	𓀎	𓀏𓀀	𓀁	𓀂	𓀃	𓀄	𓀅	𓀆	𓀇	𓀈	𓀉	𓀊	𓀋	𓀌	𓀍	𓀎	𓀏drxdtfcghjlnk;𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	𒁮	more我 iuv 白醋和八部𒁫doing? ΪΫάέήίΰϊ的撒下ⲧϋόύώϏϐϑ𒁮	𒁮	𒁮	𒁮	𒁮𓀀	𓀁	𓀂	𓀃	𓀄	𓀅	𓀆	𓀇	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𓀈	𓀉	𓀊	𓀋	𓀌	𓀍	𓀎	𓀏𓀀	𓀁	𓀂	𓀃	𓀄	𓀅	𓀆	𓀇	𓀈	𓀉	𓀊	𓀋	𓀌	𓀍	𓀎	𓀏𓀀	𓀁	𓀂	𓀃	𓀄	𓀅	𓀆	𓀇	𓀈	𓀉	𓀊	π	ρ	ς	σ	τ	υ	φ	χ	ψ	ω	ϊ	ϋ	ό	ύ	ώ	Ϗ𓀋	𓀌	𓀍	𓁰	𓁱	𓁲	𓁳	𓁴	𓁵	𓁶	𓁷	𓁸	𓁹	𓁺	𓁻	𓁼	𓁽	𓁾	𓁿𓁰	𓁱	𓁲	𓁳	𓁴	𓁵	𓁶	𓁷	𓁸	𓁹	𓁺	𓁻	𓁼	𓁽	𓁾	𓁿𓁰	𓁱	𓁲	𓁳	𓁴	𓁵	𓁶	𓁷	𓁸	𓁹	𓁺	𓁻	𓁼	𓁽	𓁾	𓁿𓁰	𓁱	𓁲	𓁳	𓁴	𓁵	𓁶	𓁷	𓁸	𓁹	𓁺	𓁻	𓁼	𓁽	𓁾	𓁿𓁰	𓁱	𓁲	𓁳	𓁴	𓁵	𓁶	𓁷	𓁸	𓁹	𓁺	𓁻	𓁼	𓁽	𓁾	𓁿𓁰	𓁱	𓁲	𓁳	𓁴	𓁵	𓁶	𓁷	𓁸	𓁹	𓁺	𓁻	𓁼	𓁽	𓁾	𓁿𓁰	𓁱	𓁲	𓁳	𓁴	𓁵	𓁶	𓁷	𓁸	𓁹	𓁺	𓁻	𓁼	𓁽	𓁾	𓁿𓁰	𓁱	𓁲	𓁳ΰ	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	𫞔	";
#endif

#define FOR(n) for (int i = 0; i < 1; i++)

#define fEncoding kCFStringEncodingUTF8
//#define fEncoding NSUTF8StringEncoding
#define xEncoding kCXStringEncodingUTF8
#define xEndian   CXEndianCurrent()

    CFAbsoluteTime axtime1 = CFAbsoluteTimeGetCurrent();
    CXStringRef xs;
    FOR(50) xs = CXStringCreateWithUTF8String((UInt8 *)s);
    axtime1 = CFAbsoluteTimeGetCurrent() - axtime1;
    printf("cxc %f\n", axtime1);

    CFAbsoluteTime aftime1 = CFAbsoluteTimeGetCurrent();
    //NSString *fs;
    //FOR(50) fs = [NSString stringWithCString:s encoding:NSUTF8StringEncoding];
    CFStringRef fs;
    FOR(50) fs = CFStringCreateWithCString(kCFAllocatorSystemDefault, s, kCFStringEncodingUTF8);
    aftime1 = CFAbsoluteTimeGetCurrent() - aftime1;
    printf("cfc %f\n", aftime1);

    CFIndex fSize = 0;
    CXSize xSize = 0;

    CXSize xBufferSize = CXStringGetByteLengthWithEncoding(xs, xEncoding);
    CFIndex fBufferSize = CFStringGetMaximumSizeForEncoding(CFStringGetLength(fs), fEncoding);
    //CFIndex fBufferSize = [fs lengthOfBytesUsingEncoding:fEncoding];

    UInt32 *xBuffer = malloc(xBufferSize);
    UInt32 *fBuffer = malloc(fBufferSize);

    CFAbsoluteTime aftime = CFAbsoluteTimeGetCurrent();
    FOR(50) CFStringGetBytes(fs, CFRangeMake(0, CFStringGetLength(fs)), fEncoding, 0xFF, false, (UInt8 *)fBuffer, fBufferSize, &fSize);
    //FOR(50) [fs getBytes:fBuffer maxLength:fBufferSize usedLength:(NSUInteger *)&fSize encoding:fEncoding options:0 range:NSMakeRange(0, [fs length]) remainingRange:nil];
    aftime = CFAbsoluteTimeGetCurrent() - aftime;
    printf("cf %f\n", aftime);

    CFAbsoluteTime axtime = CFAbsoluteTimeGetCurrent();
    FOR(50) CXStringCopyWithEncoding(xs, xBuffer, xBufferSize, &xSize, xEncoding, false, xEndian);
    axtime = CFAbsoluteTimeGetCurrent() - axtime;
    printf("cx %f\n", axtime);

    printf("fS: %ld, xS: %llu, [fEst: %ld, xEst: %llu]\n", fSize, xSize, fBufferSize, xBufferSize);
    printf("%d\n", memcmp(xBuffer, fBuffer, fSize));

    return(0);
}
