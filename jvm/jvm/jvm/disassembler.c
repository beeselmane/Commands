#include "disassembler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef char *(*JVMDisassemblyFunction)(UInt8 *buffer, UInt64 *offset);

char *const ops[0x100] = {
    "nop",          "aconst_null",     "aconst_m1",     "iconst_0",     "iconst_1",  "iconst_2",       "iconst_3",      "iconst_4",
    "iconst_5",     "lconst_0",        "lconst_1",      "fconst_0",     "fconst_1",  "fconst_2",       "dconst_0",      "dconst_1",
    "bipush",       "sipush",          "ldc",           "ldc_w",        "ldc2_w",    "iload",          "lload",         "fload",
    "dload",        "aload",           "iload_0",       "iload_1",      "iload_2",   "iload_3",        "lload_0",       "lload_1",
    "lload_2",      "lload_3",         "fload_0",       "fload_1",      "fload_2",   "fload_3",        "dload_0",       "dload_1",
    "dload_2",      "dload_3",         "aload_0",       "aload_1",      "aload_2",   "aload_3",        "iaload",        "laload",
    "faload",       "daload",          "aaload",        "baload",       "caload",    "saload",         "istore",        "lstore",
    "fstore",       "dstore",          "astore",        "istore_0",     "istore_1",  "istore_2",       "istore_3",      "lstore_0",
    "lstore_1",     "lstore_2",        "lstore_3",      "fstore_0",     "fstore_1",  "fstore_2",       "fstore_3",      "dstore_0",
    "dstore_1",     "dstore_2",        "dstore_3",      "astore_0",     "astore_1",  "astore_2",       "astore_3",      "iastore",
    "lastore",      "fastore",         "dastore",       "aastore",      "bastore",   "castore",        "sastore",       "pop",
    "pop2",         "dup",             "dup_x1",        "dup_x2",       "dup2",      "dup2_x1",        "dup2_x2",       "swap",
    "iadd",         "ladd",            "fadd",          "dadd",         "isub",      "lsub",           "fsub",          "dsub",
    "imul",         "lmul",            "fmul",          "dmul",         "idiv",      "ldiv",           "fdiv",          "ddiv",
    "irem",         "lrem",            "fream",         "drem",         "ineg",      "lneg",           "fneg",          "dneg",
    "ishl",         "lshl",            "ishr",          "lshr",         "iushr",     "lushr",          "iand",          "land",
    "ior",          "lor",             "ixor",          "lxor",         "iinc",      "i2l",            "i2f",           "i2d",
    "l2i",          "l2f",             "l2d",           "f2i",          "f2l",       "f2d",            "d2i",           "d2l",
    "d2f",          "i2b",             "i2c",           "i2s",          "lcmp",      "fcmpl",          "fcmpg",         "dcmpl",
    "dcmpg",        "ifeq",            "ifne",          "iflt",         "ifge",      "ifgt",           "ifle",          "if_icmpeq",
    "if_icmpne",    "if_icmplt",       "if_icmpge",     "if_icmpgt",    "if_icmple", "if_acmpeq",      "if_acmpne",     "goto",
    "jsr",          "ret",             "tableswitch",   "lookupswitch", "ireturn",   "lreturn",        "freturn",       "dreturn",
    "areturn",      "return",          "getstatic",     "putstatic",    "getfield",  "putfield",       "invokevirtual", "invokespecial",
    "invokestatic", "invokeinterface", "invokedynamic", "new",          "newarray",  "anewarray",      "arraylength",   "athrow",
    "checkcast",    "instanceof",      "monitorenter",  "monitorexit",  "wide",      "multianewarray", "ifnull",        "ifnonnull",
    "goto_w",       "jsr_w",           "breakpoint",    "<error>",      "<error>",   "<error>",        "<error>",       "<error>",
    "<error>",      "<error>",         "<error>",       "<error>",      "<error>",   "<error>",        "<error>",       "<error>",
    "<error>",      "<error>",         "<error>",       "<error>",      "<error>",   "<error>",        "<error>",       "<error>",
    "<error>",      "<error>",         "<error>",       "<error>",      "<error>",   "<error>",        "<error>",       "<error>",
    "<error>",      "<error>",         "<error>",       "<error>",      "<error>",   "<error>",        "<error>",       "<error>",
    "<error>",      "<error>",         "<error>",       "<error>",      "<error>",   "<error>",        "impdep1",       "impdep2"
};

char *JVMDisassembleStandard(UInt8 *buffer, UInt64 *offset)
{
    char *base = ops[buffer[*offset]];
    char *result = null;
    (*offset)++;

    asprintf(&result, "%s", )
    return base;
}

char *JVMDisassembleWide(UInt8 *buffer, UInt64 *offset)
{
    char *base = ops[buffer[*offset]];

    if (buffer[*offset] == 0x84) {
        (*offset) += 6;
    } else {
        (*offset) += 4;
    }

    return base;
}

char *JVMDisassemblePushN(UInt8 *buffer, UInt64 *offset)
{
    char *base = ops[buffer[*offset]];
    UInt8 opcode = buffer[*offset];
    char *result = null;
    (*offset)++;

    if (opcode == 0x10) {
        UInt8 byte = buffer[*offset];

        asprintf(&result, "%s %d", base, (SInt32)byte);
        (*offset)++;
    } else {
        UInt8 byte0 = buffer[(*offset) + 0];
        UInt8 byte1 = buffer[(*offset) + 1];

        asprintf(&result, "%s %d", base, (SInt32)((byte0 << 8) | byte1));
        (*offset) += 2;
    }

    return result;
}

char *JVMDisassembleLDC(UInt8 *buffer, UInt64 *offset)
{
    char *base = ops[buffer[*offset]];
    UInt8 opcode = buffer[*offset];
    char *result = null;
    (*offset)++;

    switch (opcode)
    {
        case 0x12: {
            UInt8 byte = buffer[(*offset)];

            asprintf(&result, "%s %d", base, (SInt16)(byte - 1));
            (*offset)++;
        } break;
        case 0x13:
        case 0x14: {
            UInt8 byte0 = buffer[(*offset) + 0];
            UInt8 byte1 = buffer[(*offset) + 1];

            asprintf(&result, "%s %d", base, (SInt16)(((byte0 << 8) | byte1) - 1));
            (*offset) += 2;
        } break;
    }

    return result;
}

char *JVMDisassembleLoad(UInt8 *buffer, UInt64 *offset)
{
    char *base = ops[buffer[*offset]];
    char *result = null;
    (*offset)++;

    asprintf(&result, "%s %d", base, (SInt32)(buffer[*offset]));
    (*offset)++;

    return result;
}

char *JVMDisassemble5(UInt8 *buffer, UInt64 *offset)
{
    char *base = ops[buffer[*offset]];
    (*offset) += 5;
    return base;
}

char *JVMDisassemble4(UInt8 *buffer, UInt64 *offset)
{
    char *base = ops[buffer[*offset]];
    (*offset) += 4;
    return base;
}

char *JVMDisassemble3(UInt8 *buffer, UInt64 *offset)
{
    char *base = ops[buffer[*offset]];
    (*offset) += 3;
    return base;
}

char *JVMDisassemble2(UInt8 *buffer, UInt64 *offset)
{
    char *base = ops[buffer[*offset]];
    (*offset) += 2;
    return base;
}

char *JVMDisassembleError(UInt8 *buffer, UInt64 *offset)
{
    exit(1);
}

#define std JVMDisassembleStandard
#define da2 JVMDisassemble2
#define da3 JVMDisassemble3
#define da4 JVMDisassemble4
#define da5 JVMDisassemble5
#define wid JVMDisassembleWide
#define pus JVMDisassemblePushN
#define ldc JVMDisassembleLDC

#define err JVMDisassembleError

JVMDisassemblyFunction disassemble[0x100] = {
    &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, // 00-0F
    &pus, &pus, &ldc, &ldc, &ldc, &da2, &da2, &da2, &da2, &da2, &std, &std, &std, &std, &std, &std, // 10-1F
    &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, // 20-2F
    &std, &std, &std, &std, &std, &std, &da2, &da2, &da2, &da2, &da2, &std, &std, &std, &std, &std, // 30-3F
    &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, // 40-4F
    &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, // 50-5F
    &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, // 60-6F
    &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, // 70-7F
    &std, &std, &std, &std, &da3, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, &std, // 80-8F
    &std, &std, &std, &std, &std, &std, &std, &std, &std, &da3, &da3, &da3, &da3, &da3, &da3, &da3, // 90-9F
    &da3, &da3, &da3, &da3, &da3, &da3, &da3, &da3, &da3, &da2, &err, &err, &std, &std, &std, &std, // A0-AF
    &std, &std, &da3, &da3, &da3, &da3, &da3, &da3, &da3, &da5, &da5, &da3, &da2, &da3, &std, &std, // B0-BF
    &da3, &da3, &std, &std, &wid, &da4, &da3, &da3, &da5, &da5, &std, &err, &err, &err, &err, &err, // C0-CF
    &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, // D0-DF
    &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, // E0-EF
    &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &err, &std, &std  // F0-FF
};

typedef struct {
    char  *string;
    UInt64 size;
} JVMLineList;

char *JVMDisassemble(UInt8 *program, UInt64 size)
{
    if (!program || !size) return null;
    UInt64 offset = 0;

    JVMLineList *lines = malloc(size * sizeof(JVMLineList));
    memset(lines, null, size * sizeof(JVMLineList));
    UInt64 finalSize = 0;
    char *final = null;
    UInt64 i = 0;

    do {
        JVMLineList *line = &lines[i++];
        line->string = disassemble[program[offset]](program, &offset);
        line->size = strlen(line->string);

        finalSize += line->size + 1;
    } while (offset < size);

    final = malloc(finalSize++);
    offset = 0;

    for (UInt64 j = 0; j < i; j++)
    {
        memcpy(final + offset, lines[j].string, lines[j].size);
        offset += lines[j].size;

        final[offset] = '\n';
        offset++;
    }

    free(lines);
    return final;
}
