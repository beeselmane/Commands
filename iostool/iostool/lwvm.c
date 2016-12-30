#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define kLWVMPartitionAttributeEncrypted    0x1000000000000
#define kLWVMDiskCheckValue                 0xF000
#define kLWVMMagicRegular                   ((uid_bytes){0x6A, 0x90, 0x88, 0xCF, 0x8A, 0xFD, 0x63, 0x0A, 0xE3, 0x51, 0xE2, 0x48, 0x87, 0xE0, 0xB9, 0x8B})
#define kLWVMMagicNoChecksum                ((uid_bytes){0xB1, 0x89, 0xA5, 0x19, 0x4F, 0x59, 0x4B, 0x1D, 0xAD, 0x44, 0x1E, 0x12, 0x7A, 0xAF, 0x45, 0x39})
#define kLWVMPartitionTypeHFS               ((uid_bytes){0x48, 0x46, 0x53, 0x00, 0x00, 0x00, 0x11, 0xAA, 0xAA, 0x11, 0x00, 0x30, 0x65, 0x43, 0xEC, 0xAC})

typedef uint8_t uid_bytes[16];

typedef struct {
    uid_bytes type;
    uid_bytes guid;
    uint64_t start_offset;
    uint64_t end_offset;
    uint64_t attributes;
    UniChar name[36];
} __attribute__((packed)) LWVMPartition;

typedef struct {
    uid_bytes magic;
    uid_bytes guid;
    uint64_t full_size;
    uint32_t partition_count;
    uint32_t checksum;
    uint8_t  unknown[464];
    LWVMPartition partitions[12];
    uint16_t chunks[1024];
} __attribute__((packed)) LWVMDisk;

char *uid_to_string(uid_bytes bytes)
{
    CFUUIDBytes uuid_bytes; memcpy(&uuid_bytes, bytes, sizeof(uid_bytes));
    CFUUIDRef uuid = CFUUIDCreateFromUUIDBytes(kCFAllocatorSystemDefault, uuid_bytes);
    CFStringRef string = CFUUIDCreateString(kCFAllocatorSystemDefault, uuid);
    CFRelease(uuid);

    uint64_t length = CFStringGetLength(string);
    char *result = malloc(length + 1);
    CFStringGetBytes(string, CFRangeMake(0, length), kCFStringEncodingUTF8, 0xFF, false, (UInt8 *)result, length + 1, NULL);
    result[length] = 0;

    CFRelease(string);
    return result;
}

void dump_partition(LWVMPartition *partition)
{
    char *guid = uid_to_string(partition->guid);
    char *type;

    char name[36 * 4];
    CFIndex used_bytes;
    CFStringRef name_string = CFStringCreateWithBytes(kCFAllocatorSystemDefault, (UInt8 *)partition->name, sizeof(partition->name), kCFStringEncodingUTF16, false);
    CFStringGetBytes(name_string, CFRangeMake(0, CFStringGetLength(name_string)), kCFStringEncodingUTF8, 0xFF, false, (UInt8 *)name, sizeof(name), &used_bytes);
    name[used_bytes] = 0;

    if (!memcmp(partition->type, kLWVMPartitionTypeHFS, sizeof(uid_bytes))) {
        type = "HFS+";
    } else {
        type = "<unknown>";
    }

    printf("{\n");
    printf("        Name: %s\n", name);
    printf("        GUID: %s\n", guid);
    printf("        type: %s\n", type);
    printf("        start offset: %llu\n", partition->start_offset);
    printf("        end offset: %llu\n", partition->end_offset);
    printf("        encrypted: %s\n", ((partition->attributes & kLWVMPartitionAttributeEncrypted) ? "YES" : "NO"));
    printf("    }, ");

    free(guid);
}

void dump_disk(LWVMDisk *disk)
{
    char *guid = uid_to_string(disk->guid);
    const char *type;

    if (!memcmp(disk->magic, kLWVMMagicRegular, sizeof(uid_bytes))) {
        type = "<LWVM Partition Scheme>";
    } else if (!memcmp(disk->magic, kLWVMMagicNoChecksum, sizeof(uid_bytes))) {
        type = "<LWVM Partition Scheme (No Checksum)>";
    } else {
        printf("Error: Invalid Disk Magic!\n");
        free(guid);
        return;
    }

    printf("Disk Information:\n");
    printf("    Magic: %s\n", type);
    printf("    GUID: %s\n", guid);
    printf("    Full Size: %llu\n", disk->full_size);
    printf("    Partition Count: %d\n", disk->partition_count);
    printf("    Checksum (CRC-32): 0x%08X\n", disk->checksum);
    printf("    Partitions: ");

    for (uint32_t i = 0; i < disk->partition_count; i++)
        dump_partition(&disk->partitions[i]);

    printf("\b\b\n");
    free(guid);
}

void do_lwvm(void)
{
    const char *root_disk = "/dev/rdisk0";

    uid_t id = geteuid();
    printf("is user id %d\n", id);

    printf("Reading System Partition Information from %s...\n", root_disk);
    LWVMDisk *disk = malloc(sizeof(LWVMDisk));

    FILE *fp = fopen(root_disk, "r");

    if (!fp)
    {
        printf("Error opening disk!\n");
        return;
    }

    ssize_t s = fread(disk, sizeof(LWVMDisk), 1, fp);
    fclose(fp);

    if (s != 1)
    {
        printf("Error reading disk!\n");
        return;
    }

    dump_disk(disk);
    free(disk);
}
