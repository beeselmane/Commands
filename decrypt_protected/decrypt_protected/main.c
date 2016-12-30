#include <corecrypto/ccblowfish.h>
#include <mach-o/loader.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

static const unsigned char plain_key[65] = "ourhardworkbythesewordsguardedpleasedontsteal(c)AppleComputerInc";

static void decrypt_page(void *input, void *output)
{
    uint8_t iv[32];
    memset(iv, 0, 32);

    cccbc_one_shot(ccblowfish_cbc_decrypt_mode(), 64, plain_key, iv, (PAGE_SIZE / cccbc_block_size(ccblowfish_cbc_decrypt_mode())), input, output);
}

int main(int argc, const char *const *argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <program to patch>\n", argv[0]);

        exit(1);
    }

    struct stat *stats = malloc(sizeof(struct stat));
    if (stat(argv[1], stats)) exit(1);

    int fd = open(argv[1], O_RDWR);
    if (fd == -1) exit(1);

    void *file = mmap(NULL, stats->st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file == MAP_FAILED) exit(1);

    struct mach_header_64 *header = (struct mach_header_64 *)file;
    if (header->cputype != CPU_TYPE_X86_64) exit(1);
    if (header->magic != MH_MAGIC_64) exit(1);

    off_t offset = sizeof(struct mach_header_64);

    for (uint32_t i = 0; i < header->ncmds; i++)
    {
        struct load_command *command = (file + offset);

        if (command->cmd == LC_SEGMENT_64)
        {
            struct segment_command_64 *segment_info = (struct segment_command_64 *)command;

            if (segment_info->flags & SG_PROTECTED_VERSION_1)
            {
                printf("Decrypting Segment '%s'...\n", segment_info->segname);
                size_t encrypted_size = segment_info->filesize;
                off_t encrypted_offset = segment_info->fileoff;

                if (!encrypted_offset)
                {
                    encrypted_offset = (PAGE_SIZE * 3);
                    encrypted_size -= PAGE_SIZE;
                }

                if (encrypted_size % PAGE_SIZE)
                {
                    printf("Failure\n");
                    fprintf(stderr, "Error: Size of Segment '%s' is not a multiple of Page Size (0x%X)\n", segment_info->segname, PAGE_SIZE);
                    exit(1);
                }

                off_t current_offset = encrypted_offset;
                void *raw_page = malloc(PAGE_SIZE);

                printf("Decrypting %lu Pages...\n", (encrypted_size / PAGE_SIZE));

                while (current_offset < (encrypted_offset + encrypted_size))
                {
                    printf("Decrypting Page @ 0x%04X\n", (uint16_t)current_offset);
                    decrypt_page(file + current_offset, raw_page);
                    memcpy(file + current_offset, raw_page, PAGE_SIZE);

                    current_offset += PAGE_SIZE;
                }

                segment_info->flags ^= SG_PROTECTED_VERSION_1;
                printf("Success!\n");
                free(raw_page);

                munmap(file, stats->st_size);
                free(stats);
                close(fd);
                exit(0);
            }
        }

        offset += command->cmdsize;
    }

    munmap(file, stats->st_size);
    free(stats);
    close(fd);

    return(0);
}
