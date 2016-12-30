#include "class.h"

#include <string.h>
#include <stdlib.h>

#include <stdio.h>

#define swap16(s) __builtin_bswap16((s))
#define swap32(d) __builtin_bswap32((d))

#define sf32(s, f) s->f = swap32(s->f)
#define sf16(s, f) s->f = swap16(s->f)

#define read16(b) swap16(*((UInt16 *)(b)))
#define read32(b) swap32(*((UInt32 *)(b)))

#define readFrom16(b, o)        \
    read16(b + o);              \
    o += 2

#define readFrom32(b, o)        \
    read32(b + o);              \
    o += 4

#define as64(dH, dL) (SInt64)((((SInt64)dH) << 32) | ((SInt64)dL))

static const char *const gJVMAttributeNames[kJVMNumAttributes] = {
    "ConstantValue",
    "Code",
    "StackMapTable",
    "Exceptions",
    "BootstrapMethods"
};

UInt8 JVMGetAttributeType(JVMAttributeSource *attribute, JVMClassSource *class)
{
    for (UInt8 i = 0; i < 5; i++)
    {
        if (class->attributeNamesFound[i]){
            if (class->attributeNameIndexes[i] == attribute->nameIndex)
                return i;
        } else {
            UInt8 *name = class->constantPool[attribute->nameIndex].utf8.string;

            if (!memcmp(name, gJVMAttributeNames[i], strlen(gJVMAttributeNames[i])))
            {
                class->attributeNameIndexes[i] = attribute->nameIndex;
                class->attributeNamesFound[i] = 1;

                return i;
            }
        }
    }

    return 0xFF;
}

JVMClassSource *JVMClassSourceFromBuffer(void *address, size_t size)
{
    JVMClassSource *class = malloc(sizeof(JVMClassSource));
    memcpy(class, address, sizeof(JVMClassSource));
    memset(&class->attributeNamesFound, 0, kJVMNumAttributes);
    UInt8 *buffer = (UInt8 *)address;
    UInt64 offset = 0;

    sf32(class, magic);
    sf16(class, major);
    sf16(class, minor);

    printf("%X\n", class->magic);
    printf("%d.%d\n", class->major, class->minor);

    if (class->magic != kJVMClassMagic) goto fail;
    if (class->major > 52) goto fail;

    sf16(class, constantPoolCount);
    class->constantPoolCount--;
    offset = 10;

    if (class->constantPoolCount) {
        class->constantPool = malloc(sizeof(JVMConstantPool) * class->constantPoolCount);

        for (UInt16 i = 0; i < class->constantPoolCount; i++)
        {
            JVMConstantPool *pool = &class->constantPool[i];
            pool->tag = buffer[offset++];
            printf("%d: ", i);

            switch (pool->tag)
            {
                case kJVMConstantPoolTypeInvokeDynamic: {
                    pool->invokeDynamic.bootstrapIndex = readFrom16(buffer, offset);
                    pool->invokeDynamic.nameAndTypeIndex = -1 + readFrom16(buffer, offset);
                    printf("InvokeDynamic {%d, %d}", pool->invokeDynamic.bootstrapIndex, pool->invokeDynamic.nameAndTypeIndex);
                } break;
                case kJVMConstantPoolTypeMethodTypes: {
                    pool->methodTypes.descriptorIndex = -1 + readFrom16(buffer, offset);
                    printf("MethodTypes {%d}", pool->methodTypes.descriptorIndex);
                } break;
                case kJVMConstantPoolTypeMethodHandle: {
                    pool->methodHandle.type = buffer[offset++];
                    pool->methodHandle.index = -1 + readFrom16(buffer, offset);
                    printf("MethodHandle {%d, %d}", pool->methodHandle.type, pool->methodHandle.index);
                } break;
                case kJVMConstantPoolTypeNameAndType: {
                    pool->nameAndType.nameIndex = -1 + readFrom16(buffer, offset);
                    pool->nameAndType.typeIndex = -1 + readFrom16(buffer, offset);
                    printf("NameAndType {%d, %d}", pool->nameAndType.nameIndex, pool->nameAndType.typeIndex);
                } break;
                case kJVMConstantPoolTypeIntefaceMethod: {
                    pool->interfaceMethod.classIndex = -1 + readFrom16(buffer, offset);
                    pool->interfaceMethod.nameAndTypeIndex = -1 + readFrom16(buffer, offset);
                    printf("InterfaceMethod {%d, %d}", pool->interfaceMethod.classIndex, pool->interfaceMethod.nameAndTypeIndex);
                } break;
                case kJVMConstantPoolTypeMethod: {
                    pool->method.classIndex = -1 + readFrom16(buffer, offset);
                    pool->method.nameAndTypeIndex = -1 + readFrom16(buffer, offset);
                    printf("Method {%d, %d}", pool->method.classIndex, pool->method.nameAndTypeIndex);
                } break;
                case kJVMConstantPoolTypeField: {
                    pool->field.classIndex = -1 + readFrom16(buffer, offset);
                    pool->field.nameAndTypeIndex = -1 + readFrom16(buffer, offset);
                    printf("Field {%d, %d}", pool->field.classIndex, pool->field.nameAndTypeIndex);
                } break;
                case kJVMConstantPoolTypeString: {
                    pool->stringType.stringIndex = -1 + readFrom16(buffer, offset);
                    printf("String {%d}", pool->stringType.stringIndex);
                } break;
                case kJVMConstantPoolTypeClass: {
                    pool->classType.nameIndex = -1 + readFrom16(buffer, offset);
                    printf("Class {%d}", pool->classType.nameIndex);
                } break;
                case kJVMConstantPoolTypeDouble: {
                    pool->doubleType.highBytes = readFrom32(buffer, offset);
                    pool->doubleType.lowBytes = readFrom32(buffer, offset);
                    printf("Double {%f}", (double)as64(pool->doubleType.highBytes, pool->doubleType.lowBytes));
                } break;
                case kJVMConstantPoolTypeLong: {
                    pool->longType.highBytes = readFrom32(buffer, offset);
                    pool->longType.lowBytes = readFrom32(buffer, offset);
                    printf("Long {%lld}", as64(pool->longType.highBytes, pool->longType.lowBytes));
                } break;
                case kJVMConstantPoolTypeFloat: {
                    pool->floatType.bytes = readFrom32(buffer, offset);
                    printf("Float {%f}", (float)pool->floatType.bytes);
                } break;
                case kJVMConstantPoolTypeInteger: {
                    pool->integerType.bytes = readFrom32(buffer, offset);
                    printf("Integer {%d}", pool->integerType.bytes);
                } break;
                case kJVMConstantPoolTypeUTF8: {
                    pool->utf8.length = readFrom16(buffer, offset);
                    pool->utf8.string = malloc(pool->utf8.length + 1);
                    memcpy(pool->utf8.string, buffer + offset, pool->utf8.length);
                    offset += pool->utf8.length;

                    pool->utf8.string[pool->utf8.length] = 0;
                    printf("UTF8 {%s}", pool->utf8.string);
                } break;
                default: goto fail;
            }

            printf("\n");
        }
    } else {
        class->constantPool = null;
    }

    class->flags = readFrom16(buffer, offset);
    class->thisClass = -1 + readFrom16(buffer, offset);
    class->superClass = -1 + readFrom16(buffer, offset);
    class->interfaceCount = readFrom16(buffer, offset);

    printf("%04X\n", class->flags);
    printf("%d, %d\n", class->thisClass, class->superClass);

    printf("This Class: %s\n", class->constantPool[class->constantPool[class->thisClass].classType.nameIndex].utf8.string);
    printf("Superclass: %s\n", class->constantPool[class->constantPool[class->superClass].classType.nameIndex].utf8.string);

    if (class->interfaceCount) {
        printf("Interfaces!\n");
    } else {
        class->interfaces = null;
    }

    class->fieldCount = readFrom16(buffer, offset);

    if (class->fieldCount) {
        class->fields = malloc(sizeof(JVMFieldSource) * class->fieldCount);

        for (UInt16 i = 0; i < class->fieldCount; i++)
        {
            JVMFieldSource *field = &class->fields[i];
            field->flags = readFrom16(buffer, offset);
            field->nameIndex = -1 + readFrom16(buffer, offset);
            field->descriptorIndex = -1 + readFrom16(buffer, offset);
            field->attributeCount = readFrom16(buffer, offset);

            printf("field[%d]: %s %s\n", i, class->constantPool[field->descriptorIndex].utf8.string, class->constantPool[field->nameIndex].utf8.string);

            if (field->attributeCount) {
                field->attributes = malloc(sizeof(JVMAttributeSource) * field->attributeCount);
                printf("Attributes:\n");

                for (UInt16 j = 0; j < field->attributeCount; j++)
                {
                    JVMAttributeSource *attribute = &field->attributes[j];
                    attribute->nameIndex = -1 + readFrom16(buffer, offset);
                    attribute->length = readFrom32(buffer, offset);

                    printf("\t%s\n", class->constantPool[attribute->nameIndex].utf8.string);
                    buffer += attribute->length;
                }
            } else {
                field->attributes = null;
            }
        }
    } else {
        class->fields = null;
    }

    class->methodCount = readFrom16(buffer, offset);

    if (class->methodCount) {
        class->methods = malloc(sizeof(JVMMethodSource) * class->methodCount);

        for (UInt16 i = 0; i < class->methodCount; i++)
        {
            JVMMethodSource *method = &class->methods[i];
            method->flags = readFrom16(buffer, offset);
            method->nameIndex = -1 + readFrom16(buffer, offset);
            method->descriptorIndex = -1 + readFrom16(buffer, offset);
            method->attributeCount = readFrom16(buffer, offset);

            printf("method[%d]: %s %s\n", i, class->constantPool[method->descriptorIndex].utf8.string, class->constantPool[method->nameIndex].utf8.string);

            if (method->attributeCount) {
                method->attributes = malloc(sizeof(JVMAttributeSource) * method->attributeCount);
                printf("Attributes:\n");

                for (UInt16 j = 0; j < method->attributeCount; j++)
                {
                    JVMAttributeSource *attribute = &method->attributes[j];
                    attribute->nameIndex = -1 + readFrom16(buffer, offset);
                    attribute->length = readFrom32(buffer, offset);

                    UInt8 type = JVMGetAttributeType(attribute, class);

                    switch (type)
                    {
                        case 0xFF: {
                            printf("\t<Unknown Type '%s'>\n", class->constantPool[attribute->nameIndex].utf8.string);
                            buffer += attribute->length;
                        } break;
                        case kJVMAttributeTypeConstantValue: {
                            printf("\tConstant Value\n");
                        } break;
                        case kJVMAttributeTypeCode: {
                            printf("\tCode\n");

                            attribute->code.maxStack = readFrom16(buffer, offset);
                            attribute->code.maxLocals = readFrom16(buffer, offset);
                            attribute->code.codeLength = readFrom32(buffer, offset);
                            attribute->code.code = malloc(attribute->code.codeLength);

                            for (UInt32 k = 0; k < attribute->code.codeLength; k++)
                            {
                                attribute->code.code[k] = buffer[offset++];
                                printf("%02X ", attribute->code.code[k]);
                            }

                            printf("\n");
                            // memcpy(attribute->code.code, buffer + offset, attribute->code.codeLength);
                            // offset += attribute->code.codeLength;

                            attribute->code.exceptionTableLength = readFrom16(buffer, offset);

                            if (attribute->code.exceptionTableLength) {
                                attribute->code.exceptionTable = malloc(sizeof(JVMExceptionTable) * attribute->code.exceptionTableLength);

                                for (UInt16 k = 0; k < attribute->code.exceptionTableLength; k++)
                                {
                                    JVMExceptionTable *entry = &attribute->code.exceptionTable[k];
                                    entry->startPC = readFrom16(buffer, offset);
                                    entry->endPC = readFrom16(buffer, offset);
                                    entry->handlerPC = readFrom16(buffer, offset);
                                    entry->typeCaught = readFrom16(buffer, offset);
                                }
                            } else {
                                attribute->code.exceptionTable = null;
                            }

                            attribute->code.attributeCount = readFrom16(buffer, offset);

                            if (attribute->code.attributeCount) {
                                attribute->code.attributes = malloc(sizeof(JVMAttributeSource) * attribute->code.attributeCount);

                                for (UInt16 k = 0; k < attribute->code.attributeCount; k++)
                                {
                                    JVMAttributeSource *attr = &attribute->code.attributes[k];
                                    attr->nameIndex = -1 + readFrom16(buffer, offset);
                                    attr->length = readFrom32(buffer, offset);
                                    offset += attr->length;
                                }
                            } else {
                                attribute->code.attributes = null;
                            }
                        } break;
                        case kJVMAttributeTypeExceptions: {
                            printf("\tExceptions\n");
                        } break;
                        case kJVMAttributeTypeBootstrapMethods: {
                            printf("\tBootstrapMethods\n");
                        } break;
                    }
                }
            } else {
                method->attributes = null;
            }
        }
    } else {
        class->methodCount = null;
    }

    class->attributeCount = readFrom16(buffer, offset);

    if (class->attributeCount) {
        class->attributes = malloc(sizeof(JVMAttributeSource) * class->attributeCount);

        for (UInt16 i = 0; i < class->attributeCount; i++)
        {
            JVMAttributeSource *attribute = &class->attributes[i];
            attribute->nameIndex = readFrom16(buffer, offset);
            attribute->length = readFrom32(buffer, offset);

            printf("attributes[%d]: %s\n", i, class->constantPool[attribute->nameIndex].utf8.string);
            offset += attribute->length;
        }
    } else {
        class->attributes = null;
    }

    return class;

fail:
    free(class);
    return null;
}
