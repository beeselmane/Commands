#include "unistring.h"

extern CXStringIteratorRef CXStringIteratorCreateNoRetain(CXStringRef string);

typedef void (*CXDestroyFunction)(void *object);

struct __CXString {
    int refcount;
    CXDestroyFunction destroy;

    CXStringEncoding internalEncoding;
    CXSize bufferSize;
    CXSize length;
    void *buffer;

    CXStringEncoding smallestEncoding;
    CXStringIteratorRef iterator;
};

struct __CXStringIterator {
    int refcount;
    CXDestroyFunction destroy;

    CXStringRef string;
    CXIndex nextIndex;
    CXIndex bufferPosition;
    bool releaseString;
};

static const CXUTF8Char kCXUTF8TrailingBytes[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};

static const CXUTF8Char kCXUTF8FirstByteMark[7] = {
    0x00,
    0x00,
    0xC0,
    0xE0,
    0xF0,
    0xF8,
    0xFC
};

static const CXUTF32Char kCXUTF8ResultOffsets[6] = {
    0,
    (0xC0 <<  6) | (0x80 <<  0),
    (0xE0 << 12) | (0x80 <<  6) | (0x80 <<  0),
    (0xF0 << 18) | (0x80 << 12) | (0x80 <<  6) | (0x80 <<  0),
    (0xF8 << 24) | (0x80 << 18) | (0x80 << 12) | (0x80 <<  6) | (0x80 << 0),
    0x82082080
}; // Note: the last 2 elements will never be used with the current UTF-8 decoding algorithm.
   // Serialized code points of > 4 characters are disallowed

CXEndian CXEndianCurrent(void)
{
#if defined(__x86_64__) || defined(__i386__) || defined(__arm__)
    return kCXEndianLittle;
#else
    return kCXEndianBig;
#endif
}

CXStringEncoding CXStringDefaultEncoding(void)
{
    return kCXStringEncodingUnicode;
}

void CXStringDestroy(CXStringRef string)
{
    CXRelease(string->iterator);
    free(string->buffer);
    free(string);
}

CXStringRef CXStringCreateWithBytes(const void *bytes, CXSize length, CXStringEncoding encoding, bool matchesNativeEndian)
{
    CXStringRef string = malloc(sizeof(struct __CXString));

    switch (encoding)
    {
        case kCXStringEncodingUTF32:
        case kCXStringEncodingUnicode:
            string->length = length / sizeof(CXUTF32Char);
            string->bufferSize = length;
            string->buffer = malloc(length);
            memcpy(string->buffer, bytes, length);
            string->internalEncoding = kCXStringEncodingUTF32;

            if (!matchesNativeEndian)
            {
                CXUTF32Char *buffer = (CXUTF32Char *)string->buffer;

                for (CXIndex i = 0; i < string->length; i++)
                    CXSwap32(buffer[i]);
            }
        break;
        case kCXStringEncodingUTF16: {
            CXSize bufferCount = length / sizeof(CXUTF16Char);
            string->buffer = malloc(sizeof(CXUTF32Char) * bufferCount);
            CXUTF16Char *buffer = (CXUTF16Char *)bytes;
            string->length = 0;

            for (CXIndex i = 0; i < bufferCount; i++)
            {
                CXUTF16Char point = buffer[i];
                if (!matchesNativeEndian) CXSwap16(point);

                if (0xD7FF < point && point < 0xDC00) {
                    i++;

                    if (!(i < bufferCount))
                    {
                        // please learn to make valid utf-16
                        // high surragate without a low pair
                        free(string->buffer);
                        free(string);

                        return NULL;
                    }

                    CXUTF16Char lowSurrogate = buffer[i];
                    if (!matchesNativeEndian) CXSwap16(lowSurrogate);

                    if (0xDBFF > lowSurrogate || lowSurrogate > 0xE000)
                    {
                        // this is just wrong
                        // high surragate without low
                        free(string->buffer);
                        free(string);

                        return NULL;
                    }

                    CXUTF16Char highBits = point - 0xD800;
                    CXUTF16Char lowBits = lowSurrogate - 0xDC00;
                    CXUTF32Char bits = lowBits | (highBits << 10);
                    ((CXUTF32Char *)string->buffer)[string->length++] = bits + 0x10000;
                } else if (0xDBFF < point && point < 0xE000) {
                    // wtf this is bad UTF-16
                    // low surragte with no preceding high surragate
                    free(string->buffer);
                    free(string);

                    return NULL;
                } else {
                    ((CXUTF32Char *)string->buffer)[string->length++] = point;
                    string->length++;
                }
            }

            string->bufferSize = string->length * sizeof(CXUTF32Char);
            string->buffer = realloc(string->buffer, string->bufferSize);
            string->internalEncoding = kCXStringEncodingUTF32;
        } break;
        case kCXStringEncodingUTF8: {
            CXSize bufferCount = length / sizeof(CXUTF8Char);
            string->buffer = malloc(sizeof(CXUTF32Char) * bufferCount);
            CXUTF8Char *buffer = (CXUTF8Char *)bytes;
            CXIndex bytesLeft = length;
            string->length = 0;

            while (bytesLeft)
            {
                uint8_t nbytes = kCXUTF8TrailingBytes[(*buffer)];
                CXUTF32Char result = 0;

                if (--bytesLeft < nbytes)
                {
                    free(string->buffer);
                    free(string);

                    return NULL;
                }

                switch (nbytes)
                {
                    case 4: case 5:
                        free(string->buffer);
                        free(string);
                        return NULL;
                    case 3: result += *buffer++; result <<= 6;
                    case 2: result += *buffer++; result <<= 6;
                    case 1: result += *buffer++; result <<= 6;
                    case 0: result += *buffer++;
                }

                result -= kCXUTF8ResultOffsets[nbytes];
                ((CXUTF32Char *)string->buffer)[string->length++] = result;
                bytesLeft -= nbytes;
            }

            string->bufferSize = string->length * sizeof(CXUTF32Char);
            string->buffer = realloc(string->buffer, string->bufferSize);
            string->internalEncoding = kCXStringEncodingUTF32;
        } break;
        default:
            free(string);
            return NULL;
        break;
    }

    string->refcount = 0;
    string->destroy = (CXDestroyFunction)CXStringDestroy;
    string->smallestEncoding = kCXStringEncodingUnicode;
    string->iterator = CXStringIteratorCreateNoRetain(string);
    CXRetain(string);

    return string;
}

CXStringRef CXStringCreateWithUTF8String(const uint8_t *bytes)
{
    return CXStringCreateWithBytes(bytes, strlen((const char *)bytes), kCXStringEncodingUTF8, true);
}

CXSize CXStringGetLength(CXStringRef string)
{
    return string->length;
}

uint64_t CXStringGetByteLengthWithEncoding(CXStringRef string, CXStringEncoding encoding)
{
    if (encoding == string->internalEncoding) return string->bufferSize;

    switch (encoding)
    {
        case kCXStringEncodingUTF32:
            return string->length * sizeof(CXUTF32Char);
        break;
        case kCXStringEncodingUTF24:
            return string->length * sizeof(CXUTF24Char);
        break;
        case kCXStringEncodingUTF16: {
            CXUnicodePoint codePoint = 0;
            uint64_t utf16length = 0;

            while ((codePoint = CXStringIteratorNextCodePoint(string->iterator)))
            {
                if (codePoint < 0x10000) utf16length += (1 * sizeof(CXUTF16Char));
                else                     utf16length += (2 * sizeof(CXUTF16Char));
            }

            CXStringIteratorReset(string->iterator);
            return utf16length;
        } break;
        case kCXStringEncodingUTF8: {
            CXUnicodePoint codePoint = 0;
            uint64_t utf8length = 0;

            while ((codePoint = CXStringIteratorNextCodePoint(string->iterator)))
            {
                     if (codePoint < 0x00080) utf8length += (1 * sizeof(CXUTF8Char));
                else if (codePoint < 0x00800) utf8length += (2 * sizeof(CXUTF8Char));
                else if (codePoint < 0x10000) utf8length += (3 * sizeof(CXUTF8Char));
                else                          utf8length += (4 * sizeof(CXUTF8Char));
            }

            CXStringIteratorReset(string->iterator);
            return utf8length;
        } break;
        case kCXStringEncodingUnicode:
            return CXStringGetByteLengthWithEncoding(string, CXStringGetSmallestEncoding(string));
        break;
    }
}

CXStringEncoding CXStringGetSmallestEncoding(CXStringRef string)
{
    if (string->smallestEncoding != kCXStringEncodingUnicode) return string->smallestEncoding;

    CXUnicodePoint codePoint = 0;
    CXSize utf16 = 0;
    CXSize utf8  = 0;

    while ((codePoint = CXStringIteratorNextCodePoint(string->iterator)))
    {
        if (codePoint < 0x10000) utf16 += (1 * sizeof(CXUTF16Char));
        else                     utf16 += (2 * sizeof(CXUTF16Char));

             if (codePoint < 0x00080) utf8 += (1 * sizeof(CXUTF8Char));
        else if (codePoint < 0x00800) utf8 += (2 * sizeof(CXUTF8Char));
        else if (codePoint < 0x10000) utf8 += (3 * sizeof(CXUTF8Char));
        else                          utf8 += (4 * sizeof(CXUTF8Char));
    }

    CXStringIteratorReset(string->iterator);

    if (string->bufferSize == utf16)
    {
        string->smallestEncoding = kCXStringEncodingUTF32;
        return string->smallestEncoding;
    }

    if (utf16 < utf8) string->smallestEncoding = kCXStringEncodingUTF16;
    else string->smallestEncoding = kCXStringEncodingUTF8;

    return string->smallestEncoding;
}

bool CXStringEqual(CXStringRef s1, CXStringRef s2)
{
    if (s1->internalEncoding != s2->internalEncoding) return false;
    if (s1->length != s2->length) return false;
    CXUnicodePoint s1p = 0, s2p = 0;

    for ( ; ; )
    {
        s1p = CXStringIteratorNextCodePoint(s1->iterator);
        s2p = CXStringIteratorNextCodePoint(s2->iterator);

        if (s1p != s2p) return false;
        if (s1p == 0 || s2p == 0) break;
    }

    CXStringIteratorReset(s1->iterator);
    CXStringIteratorReset(s2->iterator);

    return true;
}

CXUnicodePoint CXStringGetCharacterAtIndex(CXStringRef string, CXIndex index)
{
    switch (string->internalEncoding) {
        case kCXStringEncodingUTF32: {
            CXUTF32Char *buffer = (CXUTF32Char *)string->buffer;
            return (CXUnicodePoint)*(buffer + index);
        } break;
        default: return 0; break;
    }
}

CXSize CXStringCopyWithEncoding(CXStringRef string, void *buffer, CXSize bufferSize, CXSize *bufferUsed, CXStringEncoding encoding, bool includeBOM, CXEndian endian)
{
    static const CXUnicodePoint uBOM32f = 0xFFFE0000;
    static const CXUnicodePoint uBOM16f = 0xFFFE;
    static const CXUnicodePoint uBOM8 = 0xEFBBBF;
    static const CXUnicodePoint uBOMr = 0xFEFF;

    bool nativeEndian = CXEndianCurrent() == endian;

    switch (string->internalEncoding)
    {
        case kCXStringEncodingUTF32: {
            switch (encoding)
            {
                case kCXStringEncodingUTF32: {
                    CXSize finalSize = string->length * sizeof(CXUTF32Char);
                    CXUTF32Char *resultBuffer = (CXUTF32Char *)buffer;

                    if (includeBOM) finalSize += sizeof(CXUTF32Char);
                    if (finalSize < bufferSize) break;

                    if (includeBOM)
                    {
                        (*resultBuffer) = nativeEndian ? uBOMr : uBOM32f;
                        resultBuffer++;
                    }

                    if (nativeEndian) {
                        memcpy(resultBuffer, string->buffer, string->length * sizeof(CXUTF32Char));
                    } else {
                        for (CXIndex i = 0; i < string->length; i++)
                            resultBuffer[i] = CXSwap32(((CXUTF32Char *)string->buffer)[i]);
                    }

                    if (bufferUsed) (*bufferUsed) = finalSize;
                    return string->length;
                } break;
                case kCXStringEncodingUTF16: {
                    if ((string->length * sizeof(CXUTF16Char)) > bufferSize) break;
                    CXUnicodePoint *stringBuffer = (CXUTF32Char *)string->buffer;
                    CXUTF16Char *resultBuffer = (CXUTF16Char *)buffer;
                    CXSize finalSize = 0;

                    if (includeBOM)
                    {
                        (*resultBuffer++) = nativeEndian ? uBOMr : uBOM16f;
                        bufferSize -= sizeof(CXUTF16Char);
                        finalSize++;
                    }

                    for (CXIndex i = 0; i < string->length; i++)
                    {
                        CXUnicodePoint point = stringBuffer[i];

                        if (point > 0xFFFF) {
                            if ((finalSize + 2) > bufferSize) return 0;

                            CXUTF32Char base = point - 0x10000;
                            CXUTF16Char highSurrogate = (base >> 10) + 0xD800;
                            CXUTF16Char lowSurrogate = (base & 0x3FF) + 0xDC00;

                            if (nativeEndian) {
                                resultBuffer[finalSize++] = highSurrogate;
                                resultBuffer[finalSize++] = lowSurrogate;
                            } else {
                                resultBuffer[finalSize++] = CXSwap16(highSurrogate);
                                resultBuffer[finalSize++] = CXSwap16(lowSurrogate);
                            }
                        } else {
                            if ((finalSize + 1) > bufferSize) return 0;
                            resultBuffer[finalSize++] = (nativeEndian) ? (CXUTF16Char)point : CXSwap16(point);
                        }
                    }

                    if (bufferUsed) (*bufferUsed) = finalSize * sizeof(CXUTF16Char);
                    return string->length;
                } break;
                case kCXStringEncodingUTF8: {
                    if ((string->length * sizeof(CXUTF8Char)) > bufferSize) break;
                    CXUnicodePoint *stringBuffer = (CXUTF32Char *)string->buffer;
                    CXUnicodePoint *stringBufferEnd = stringBuffer + string->length;
                    CXUTF8Char *resultBuffer = (CXUTF8Char *)buffer;
                    CXUTF8Char *resultEnd = resultBuffer + bufferSize;

                    if (includeBOM)
                    {
                        (*resultBuffer++) = uBOM8 >> 16;
                        (*resultBuffer++) = uBOM8 >> 8;
                        (*resultBuffer++) = uBOM8;

                        resultBuffer += 3;
                    }

                    while (stringBuffer < stringBufferEnd && resultBuffer < resultEnd)
                    {
                        CXUnicodePoint point = (*stringBuffer++);

                        if (point < 0x80) {
                            (*resultBuffer++) = point;
                            continue;
                        } else {
                            CXUTF8Char nbytes;

                                 if (point < 0x800)    nbytes = 2;
                            else if (point < 0x10000)  nbytes = 3;
                            else if (point < 0x200000) nbytes = 4;
                            else                       nbytes = 0;

                            switch (nbytes)
                            {
                                case 4: resultBuffer[3] = (point | 0x80) & 0xBF; point >>= 6;
                                case 3: resultBuffer[2] = (point | 0x80) & 0xBF; point >>= 6;
                                case 2: resultBuffer[1] = (point | 0x80) & 0xBF; point >>= 6;
                                case 1: resultBuffer[0] = point | kCXUTF8FirstByteMark[nbytes];
                            }

                            resultBuffer += nbytes;
                        }
                    }

                    if (bufferUsed) (*bufferUsed) = ((CXIndex)resultBuffer - (CXIndex)buffer);
                    return string->length;
                } break;
                default: break;
            }
        } default: return 0; break;
    }
}

void CXStringIteratorDestroy(CXStringIteratorRef iterator)
{
    if (iterator->releaseString) CXRelease(iterator->string);
    free(iterator);
}

CXStringIteratorRef CXStringIteratorCreateNoRetain(CXStringRef string)
{
    CXStringIteratorRef iterator = malloc(sizeof(struct __CXStringIterator));
    iterator->destroy = (CXDestroyFunction)CXStringIteratorDestroy;
    iterator->string = string;
    iterator->nextIndex = 0;
    iterator->bufferPosition = 0;
    iterator->releaseString = false;
    CXRetain(iterator);
    return iterator;
}

CXStringIteratorRef CXStringIteratorCreate(CXStringRef string)
{
    CXRetain(string);
    CXStringIteratorRef iterator = CXStringIteratorCreateNoRetain(string);
    iterator->releaseString = true;
    return iterator;
}

CXStringIteratorRef CXStringIteratorCreateAtIndex(CXStringRef string, CXIndex index)
{
    CXStringIteratorRef iterator = CXStringIteratorCreate(string);
    while (iterator->nextIndex < index) CXStringIteratorNextCodePoint(iterator);
    return iterator;
}

CXUnicodePoint CXStringIteratorNextCodePoint(CXStringIteratorRef iterator)
{
    CXUnicodePoint result = 0;

    switch (iterator->string->internalEncoding)
    {
        case kCXStringEncodingUTF32: {
            CXUTF32Char *buffer = (CXUnicodePoint *)iterator->string->buffer;
            result = (CXUnicodePoint)*(buffer + iterator->bufferPosition);
            iterator->bufferPosition++;
        } break;
        default: return 0; break;
    }

    iterator->nextIndex++;
    return result;
}

CXIndex CXStringIteratorGetNextIndex(CXStringIteratorRef iterator)
{
    return iterator->nextIndex;
}

void CXStringIteratorReset(CXStringIteratorRef iterator)
{
    iterator->bufferPosition = 0;
    iterator->nextIndex = 0;
}

void CXRetain(void *object)
{
    int *refcount = (int *)object;
    (*refcount)++;
}

void CXRelease(void *object)
{
    int *refcount = (int *)object;
    (*refcount)--;

    if (!(*refcount))
    {
        CXDestroyFunction *destroy = ((CXDestroyFunction *)object) + 1;
        (*destroy)(object);
    }
}

