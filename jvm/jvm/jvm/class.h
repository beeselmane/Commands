#ifndef __JAVA_CLASS__
#define __JAVA_CLASS__ 1

#include "java.h"

#define kJVMAttributeTypeConstantValue      0x0
#define kJVMAttributeTypeCode               0x1
#define kJVMAttributeTypeStackMapTable      0x2
#define kJVMAttributeTypeExceptions         0x3
#define kJVMAttributeTypeBootstrapMethods   0x4

#define kJVMConstantPoolTypeInvokeDynamic   18
#define kJVMConstantPoolTypeMethodTypes     16
#define kJVMConstantPoolTypeMethodHandle    15
#define kJVMConstantPoolTypeNameAndType     12
#define kJVMConstantPoolTypeIntefaceMethod  11
#define kJVMConstantPoolTypeMethod          10
#define kJVMConstantPoolTypeField           9
#define kJVMConstantPoolTypeString          8
#define kJVMConstantPoolTypeClass           7
#define kJVMConstantPoolTypeDouble          6
#define kJVMConstantPoolTypeLong            5
#define kJVMConstantPoolTypeFloat           4
#define kJVMConstantPoolTypeInteger         3
#define kJVMConstantPoolTypeUTF8            1

typedef struct {
    UInt8 tag;

    union {
        struct {
            UInt16 bootstrapIndex;
            UInt16 nameAndTypeIndex;
        } invokeDynamic;
        struct {
            UInt16 descriptorIndex;
        } methodTypes;
        struct {
            UInt8 type;
            UInt16 index;
        } methodHandle;
        struct {
            UInt16 nameIndex;
            UInt16 typeIndex;
        } nameAndType;
        struct {
            UInt16 classIndex;
            UInt16 nameAndTypeIndex;
        } interfaceMethod;
        struct {
            UInt16 classIndex;
            UInt16 nameAndTypeIndex;
        } method;
        struct {
            UInt16 classIndex;
            UInt16 nameAndTypeIndex;
        } field;
        struct {
            UInt16 stringIndex;
        } stringType;
        struct {
            UInt16 nameIndex;
        } classType;
        struct {
            UInt32 highBytes;
            UInt32 lowBytes;
        } doubleType;
        struct {
            UInt32 highBytes;
            UInt32 lowBytes;
        } longType;
        struct {
            UInt32 bytes;
        } floatType;
        struct {
            UInt32 bytes;
        } integerType;
        struct {
            UInt16 length;
            UInt8 *string;
        } utf8;
    };
} JVMConstantPool;

typedef struct {
    UInt16 startPC;
    UInt16 endPC;
    UInt16 handlerPC;
    UInt16 typeCaught;
} JVMExceptionTable;

typedef struct __JVMAttributeSource {
    UInt16 nameIndex;
    UInt32 length;

    union {
        struct {
            UInt16 index;
        } constantValue;
        struct {
            UInt16 maxStack;
            UInt16 maxLocals;
            UInt32 codeLength;
            UInt8 *code;
            UInt16 exceptionTableLength;
            JVMExceptionTable *exceptionTable;
            UInt16 attributeCount;
            struct __JVMAttributeSource *attributes;
        } code;
        struct {
            //
        } stackMapTable;
        struct {
            //
        } exceptions;
        struct {
            //
        } bootstrapMethods;
    };
} JVMAttributeSource;

typedef struct {
    UInt16 flags;
    UInt16 nameIndex;
    UInt16 descriptorIndex;
    UInt16 attributeCount;
    JVMAttributeSource *attributes;
} JVMFieldSource;

typedef struct {
    UInt16 flags;
    UInt16 nameIndex;
    UInt16 descriptorIndex;
    UInt16 attributeCount;
    JVMAttributeSource *attributes;
} JVMMethodSource;

#define kJVMClassMagic  0xCAFEBABE
#define kJVMNullByte0   0x80
#define kJVMNullByte1   0xC0

#define kJVMNumAttributes 5

typedef struct {
    UInt32 magic;
    UInt16 minor;
    UInt16 major;
    UInt16 constantPoolCount;
    JVMConstantPool *constantPool;
    UInt16 flags;
    UInt16 thisClass;
    UInt16 superClass;
    UInt16 interfaceCount;
    UInt16 *interfaces;
    UInt16 fieldCount;
    JVMFieldSource *fields;
    UInt16 methodCount;
    JVMMethodSource *methods;
    UInt16 attributeCount;
    JVMAttributeSource *attributes;

    UInt16 attributeNameIndexes[kJVMNumAttributes];
    UInt8 attributeNamesFound[kJVMNumAttributes];
} JVMClassSource;

JVMClassSource *JVMClassSourceFromBuffer(void *address, size_t size);

#endif /* !defined(__JAVA_CLASS__) */
