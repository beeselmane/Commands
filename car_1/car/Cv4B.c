#include "crc32.h"
#include "Cv4B.h"
#include "list.h"
#include "car.h"

#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define kCv4BEntryTypeDirectory kFLEntryTypeDirectory
#define kCv4BEntryTypeRegular   kFLEntryTypeRegular
#define kCv4BEntryTypeSymlink   kFLEntryTypeSymlink
#define pack                    __attribute__((packed))
#define bsize                   (1 << 28)

typedef struct {
         uint8_t magic[kCArchiveMagicSize];
         uint8_t version[kCArchiveVersionSize];
         uint8_t extraByte;
    pack uint32_t tocOffset;
    pack uint64_t stringOffset;
    pack uint64_t dataOffset;
    pack uint32_t checksum;
} Cv4BArchiveHeader;

typedef struct {
    pack uint32_t nameOffset;
         uint8_t type;
    pack uint64_t dataOffset;
    pack uint64_t size;
         uint8_t extra[3];
} Cv4BArchiveEntry;

bool Cv4BArchiveCreate(const char *archive, const char *rootdir)
{
    size_t rdlen = strlen(rootdir);

    fprintf(stderr, "Building file list...");
    FList *list = FListCreate();

    if (!FListAddDirectory(list, rootdir))
    {
        fprintf(stderr, "Error: Could not add file '%s'\n", list->tail->name);
        FListDestroy(list);

        return false;
    }

    fprintf(stderr, " (%lu)\n", list->size);
    char *tmpdir; char *arch_base = basename((char *)archive);
    asprintf(&tmpdir, "/tmp/%s.XXXXXXXXXX", arch_base);
    mkdtemp(tmpdir); free(arch_base);
    fprintf(stderr, "Writing archive parts to '%s'...\n", tmpdir);

    char *hdrfile; asprintf(&hdrfile, "%s/header", tmpdir);
    char *extfile; asprintf(&extfile, "%s/extra", tmpdir);
    char *tocfile; asprintf(&tocfile, "%s/entries", tmpdir);
    char *strfile; asprintf(&strfile, "%s/strings", tmpdir);
    char *datfile; asprintf(&datfile, "%s/data", tmpdir);

    int fd_hdr = open(hdrfile, O_CREAT | O_RDWR);
    int fd_ext = open(extfile, O_CREAT | O_RDWR);
    int fd_toc = open(tocfile, O_CREAT | O_RDWR);
    int fd_str = open(strfile, O_CREAT | O_RDWR);
    int fd_dat = open(datfile, O_CREAT | O_RDWR);

    uint8_t *buffer = malloc(bsize);
    uint32_t checksum = crc32_init();
    FListEntry *entry = list->head;
    size_t strtablen = 0;
    off_t datoff = 0;
    off_t stroff = 0;

    while (entry != list->tail->next)
    {
        Cv4BArchiveEntry tocEntry = {
            .nameOffset = (uint32_t)stroff,
            .type = entry->type,
            .dataOffset = datoff,
            .size = entry->size
        };

        char *pathname = entry->name + rdlen;
        if (entry == list->head) pathname = "/";
        checksum = crc32_update(checksum, pathname, strlen(pathname) + 1);
        ssize_t written = write(fd_str, pathname, strlen(pathname) + 1);
        strtablen += written;
        stroff += written;

        checksum = crc32_update(checksum, &tocEntry, sizeof(Cv4BArchiveEntry));
        write(fd_toc, &tocEntry, sizeof(Cv4BArchiveEntry));

        switch (entry->type) {
            case kFLEntryTypeDirectory: break;
            case kFLEntryTypeRegular: {
                int file = open(entry->name, O_RDONLY);
                ssize_t rsize = -1;

                do {
                    rsize = read(file, buffer, bsize);
                    if (!rsize) break;

                    checksum = crc32_update(checksum, buffer, rsize);
                    datoff += write(fd_dat, buffer, rsize);
                } while (rsize == bsize);

                close(file);
            } break;
            case kFLEntryTypeSymlink: {
                char *linkbuffer = malloc(entry->size + 1);
                readlink(entry->name, linkbuffer, entry->size + 1);
                linkbuffer[entry->size] = 0;

                checksum = crc32_update(checksum, linkbuffer, entry->size + 1);
                datoff += write(fd_dat, linkbuffer, entry->size + 1);
            } break;
        }

        entry = entry->next;
    }

    close(fd_toc);
    close(fd_str);
    close(fd_dat);

    struct stat tocStats; stat(tocfile, &tocStats);

    Cv4BArchiveHeader header = {
        .magic = kCv4BMagic,
        .version = kCv4BVersion,
        .extraByte = 0,
        .tocOffset = sizeof(Cv4BArchiveHeader),
        .stringOffset = tocStats.st_size,
        .dataOffset = tocStats.st_size + stroff,
        .checksum = 0
    };

    checksum = crc32_update(checksum, &header, sizeof(Cv4BArchiveHeader) - sizeof(uint32_t));
    header.checksum = crc32_finish(checksum);

    write(fd_hdr, &header, sizeof(Cv4BArchiveHeader));
    close(fd_hdr);
    close(fd_ext);

    free(hdrfile);
    free(extfile);
    free(tocfile);
    free(strfile);
    free(datfile);

    FListDestroy(list);
    free(buffer);

    fprintf(stderr, "Merging parts into '%s'...\n", archive);
    bool rval = Cv4BArchiveMerge(archive, tmpdir);
    free(tmpdir);
    return rval;
}

bool Cv4BArchiveListItems(const char *archive, const char *const *paths, size_t *listSize)
{
    return false;
}

bool Cv4BArchiveExtractItem(const char *archive, const char *path, const char *file)
{
    return false;
}

bool Cv4BArchiveExtractAllItems(const char *archive, const char *directory)
{
    return false;
}

bool Cv4BArchiveBasicValidate(const char *archive, bool *isValid)
{
    return false;
}

bool Cv4BArchiveStrictValidate(const char *archive, bool *isValid)
{
    return false;
}

bool Cv4BArchiveSplit(const char *archive, const char *directory)
{
    return false;
}

bool Cv4BArchiveMerge(const char *archive, const char *directory)
{
    if (access(archive, F_OK))
    {
        int fd = open(archive, O_CREAT);
        fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        close(fd);
        
        if (fd == -1)
        {
            fprintf(stderr, "Error: Could not create archive file '%s'\n", archive);
            return false;
        }
    }

    int fd = open(archive, O_RDWR);
    
    if (fd == -1)
    {
        fprintf(stderr, "Error: Could not open archive file '%s'\n", archive);
        return false;
    }

    return true;
}