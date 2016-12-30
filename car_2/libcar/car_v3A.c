#include <carutil.h>
#include <libcar.h>
#include <assert.h>

#ifdef DEBUG
    #define dlog(x, ...) printf(x, ##__VA_ARGS__)
#else
    #define dlog(x, ...) ((void)0)
#endif

#define CV3Exit(c, v, s)                            \
    do {                                            \
        (v) = (c);                                  \
        goto s;                                     \
    } while (0)

#define CV3RExit(c, v, s)                           \
    do {                                            \
        if ((v)) (*(v)) = (c);                      \
        goto s;                                     \
    } while (0)

// Note (IMPORTANT!): I assume 2 local variables in
// the next 2 macros A MemoryBlock pointer named `archive`
// and an integer named `offset`. These exist in functions
// that need this macro though, so it should be okay.
#define mwrite(d, s) mwriteat(offset, d, s)

#define mwriteat(o, d, s)                           \
    do {                                            \
        memcpy((archive->address + o), d, s);       \
        o += s;                                     \
    } while (0)

// For CARListAddDirectory ONLY
#define frexit(c, e)                                \
    do {                                            \
        free(fullname);                             \
        CV3Exit(c, retval, e);                      \
    } while (0)

// For writing null-terminators to strings
static const uint8_t zero = 0x00;

// For use in memcmp()
static const Version version = CAR_VERSION_v3;

// Note: directory must be checked for access before first call
static int CARListAddDirectory(CARList *list, Path directory)
{
    int retval = ERR_NONE;

    DIR *dir = opendir(directory);
    if (!dir) CV3Exit(ERR_OPENDIR, retval, done);
    struct dirent *direntry;

    while ((direntry = readdir(dir)))
    {
        if (direntry->d_namlen == 2 && *((uint16_t *)direntry->d_name) == '..') continue;
        if (direntry->d_namlen == 1 && *((uint8_t  *)direntry->d_name) == '.' ) continue;

        char *fullname;
        CARString3(fullname, directory, "/", direntry->d_name);
        struct stat stats;

        if (lstat(fullname, &stats)) frexit(ERR_LSTAT, done);

        if (S_ISDIR(stats.st_mode)) {
            if (access(fullname, F_OK | R_OK | X_OK))
            {
                dlog("Permission error on %s (isDir)\n", fullname);
                frexit(ERR_ACCESS, done);
            }

            dlog("Adding directory %s...\n", fullname);
            int rval = CARListAddDirectory(list, fullname);
            if (rval != ERR_NONE) frexit(rval, done);
        } else if (S_ISREG(stats.st_mode)) {
            if (access(fullname, F_OK | R_OK | W_OK))
            {
                dlog("Permission error on %s (isReg)\n", fullname);
                frexit(ERR_ACCESS, done);
            }

            CV3Entry e = {
                .filename = fullname,
                .filesize = stats.st_size,
                .namesize = (uint16_t)(strlen(fullname) - list->shift)
            }, *entry = malloc(sizeof(CV3Entry)); memcpy(entry, &e, sizeof(CV3Entry));

            dlog("Adding File %s:\n", fullname);
            dlog("\tShifted Name: %s\n", (entry->filename + list->shift));
            dlog("\tSize: %lu\n", entry->filesize);
            dlog("\tShifted Name Length: %hu\n", entry->namesize);
            dlog("\tEntry is at %p\n", entry);
            CAREntry *listentry = CAREntryMake(entry);

            if (list->head) {
                list->tail->next = listentry;
                list->tail = list->tail->next;
            } else {
                list->head = listentry;
                list->tail = listentry;
            }

            list->fullsize += entry->filesize;
            list->namesize += entry->namesize;
            list->size++;
        } else {
            dlog("Ignoring %s. Unknown filetype\n", fullname);
        }
    }

done:
    closedir(dir);
    return retval;
}

#undef frexit

int CV3CreateArchive(Path destpath, Path root, uint8_t flags)
{
    bool check = flags & kCV3HasChecksum;
    bool utf16 = flags & kCV3UTF16;
    bool utf32 = flags & kCV3UTF32;
    bool verbose = true;

    int retval = ERR_NONE;

    if (access(root, F_OK | R_OK | X_OK)) CV3Exit(ERR_ACCESS, retval, exitexit);
    int fd = open(destpath, O_RDWR | O_CREAT);
    chmod(destpath, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) CV3Exit(ERR_OPEN, retval, exitexit);

    CARList *list = CARListCreate();
    list->shift = strlen(root);
    int error = CARListAddDirectory(list, root);
    if (error != ERR_NONE) CV3Exit(error, retval, fdexit);
    dlog("Got %zu files.\n", list->size);
    dlog("All files are %zu bytes.\n", list->fullsize);
    dlog("Names are %zu bytes.\n", list->namesize);

    CARHeaderVersion3 header = {
        .magic = CAR_MAGIC_v3,
        .version = CAR_VERSION_v3,
        .flags = (flags & 0x0F),
        .filecount = list->size,
        .extsize = list->fullsize,
        .checksum = 0x0,
        .reserved = {0, 0, 0, 0}
    };

    if (BIG_ENDIAN) flags |= kCV3IsBigEndian;
    uint64_t metadata = sizeof(CARHeaderVersion3);
    metadata += (sizeof(CAREntryVersion3) * list->size);
    metadata += list->namesize + (list->size * 3);
    // 16 bits for namesize then 8 bits for null byte
    header.archsize = metadata + header.extsize;
    header.data = metadata;

    ftruncate(fd, header.archsize);
    MemoryBlock *archive = malloc(sizeof(MemoryBlock));
    archive->address = mmap(NULL, header.archsize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FILE, fd, 0);
    archive->size = header.data;

    if (((intptr_t)archive->address) < 0) CV3Exit(ERR_MMAP, retval, listexit);
    uint64_t dataOffset = 0;
    uint64_t offset = 0;

    mwrite(&header, sizeof(CARHeaderVersion3));
    CAREntry *centry = list->head;
    CV3Entry *dentry = NULL;

    dlog("Wrote Header to %s:\n", destpath);
    dlog("\tFile Count: %llu\n", header.filecount);
    dlog("\tRaw Size: %llu\n", header.extsize);
    dlog("\tArchive Size: %llu\n", header.archsize);
    dlog("\tData Offset: %llu\n", header.data);

    for (uint64_t i = 0; i < list->size; i++)
    {
        dentry = (CV3Entry *)centry->data;
        dlog("Adding %s at data %llu...\n", (dentry->filename + list->shift), dataOffset);
        dlog("TOC Entry is at %llu & namesize is %hu\n", offset, dentry->namesize);

        CAREntryVersion3 fentry = {
            .size = dentry->filesize,
            .offset = dataOffset
        };

        mwrite(&dentry->namesize, sizeof(uint16_t));
        mwrite((dentry->filename + list->shift), dentry->namesize);
        mwrite(&zero, sizeof(uint8_t));
        mwrite(&fentry, sizeof(CAREntryVersion3));

        FILE *fp = fopen(dentry->filename, "rb");
        if (!fp) CV3Exit(ERR_OPEN, retval, mapexit);
        fread(archive->address + (dataOffset + header.data), 1, dentry->filesize, fp);
        fclose(fp);

        dlog("Wrote %zu bytes to %llu\n", dentry->filesize, (dataOffset + header.data));
        dataOffset += dentry->filesize;
        centry = centry->next;
        free(dentry->filename);
    }

    dlog("Final TOC Offset: %llu\n", offset);
    dlog("Final Data Offset: %llu\n", dataOffset);

mapexit:
    munmap(archive->address, archive->size);
    free(archive);

listexit:
    CARListDestroy(list);

fdexit:
    close(fd);

exitexit:
    return retval;
}

FileBuffer *CV3GetFile(Path arch, Path file, int *error)
{
    MemoryBlock *archive = NULL;
    FileBuffer *filebuffer = NULL;
    bool flipendian = false;

    int fd = open(arch, O_RDONLY);
    if (fd < 0) CV3RExit(ERR_OPEN, error, exitexit);

    struct stat *stats = malloc(sizeof(struct stat));
    if (stat(arch, stats)) CV3RExit(ERR_STAT, error, fdexit);

    archive = malloc(sizeof(MemoryBlock));
    archive->address = mmap(NULL, stats->st_size, PROT_READ, MAP_SHARED | MAP_FILE, fd, 0);
    if (((intptr_t)archive->address) < 0) CV3RExit(ERR_MMAP, error, statexit);
    archive->size = stats->st_size;
    
    CARHeaderVersion3 *header = (CARHeaderVersion3 *)archive->address;
    if (!(header->magic == CAR_MAGIC_v3 || header->magic == CAR_CIGAM_v3)) CV3RExit(ERR_ARCHIVE, error, mapexit);
    if (memcmp(header->version, version, sizeof(Version))) CV3RExit(ERR_ARCHIVE, error, mapexit);
    if (header->magic == CAR_CIGAM_v3) flipendian = true;
    
    uint64_t offset = sizeof(CARHeaderVersion3);
    bool utf32 = header->flags | kCV3UTF32;
    bool utf16 = header->flags | kCV3UTF16;
    size_t filecount = header->filecount;
    
    for (uint64_t i = 0; i < filecount; i++)
    {
        uint16_t namelen = *((uint16_t *)(archive->address + offset)); offset += sizeof(uint16_t);

        if (!memcmp(file, (archive->address + offset), namelen + 1))
        {
            // Found it :D
            offset += (namelen + 1);

            CAREntryVersion3 *entry = (CAREntryVersion3 *)(archive->address + offset);
            filebuffer = malloc(sizeof(FileBuffer));
            filebuffer->address = archive->address + (header->data + entry->offset);
            filebuffer->name = calloc(strlen(file), sizeof(char));
            strcpy(filebuffer->name, file);
            filebuffer->size = entry->size;
            filebuffer->base = archive;

            filebuffer->free = (void (^)(void *))^(FileBuffer *self) {
                munmap(self->base->address, self->base->size);
                free(self->base);

                free(self->name);
                free(self);
            };

            CV3RExit(ERR_NONE, error, statexit);
        }

        offset += namelen + 1;
        offset += sizeof(CAREntryVersion3);
    }

mapexit:
    munmap(archive->address, archive->size);
    free(archive);
    
statexit:
    free(stats);
    
fdexit:
    close(fd);
    
exitexit:
    return filebuffer;
}

int CV3ExtractArchive(Path arch, Path outdir)
{
    bool flipendian = false;
    int retval = ERR_NONE;

    int fd = open(arch, O_RDONLY);
    if (fd < 0) CV3Exit(ERR_OPEN, retval, exitexit);

    struct stat *stats = malloc(sizeof(struct stat));
    if (stat(arch, stats)) CV3Exit(ERR_STAT, retval, fdexit);

    MemoryBlock *archive = malloc(sizeof(MemoryBlock));
    archive->address = mmap(NULL, stats->st_size, PROT_READ, MAP_SHARED | MAP_FILE, fd, 0);
    if (((intptr_t)archive->address) < 0) CV3Exit(ERR_MMAP, retval, statexit);
    archive->size = stats->st_size;

    CARHeaderVersion3 *header = (CARHeaderVersion3 *)archive->address;
    if (!(header->magic == CAR_MAGIC_v3 || header->magic == CAR_CIGAM_v3)) CV3Exit(ERR_ARCHIVE, retval, mapexit);
    if (memcmp(header->version, version, sizeof(Version))) CV3Exit(ERR_ARCHIVE, retval, mapexit);
    if (header->magic == CAR_CIGAM_v3) flipendian = true;

    uint64_t offset = sizeof(CARHeaderVersion3);
    bool utf32 = header->flags | kCV3UTF32;
    bool utf16 = header->flags | kCV3UTF16;
    size_t filecount = header->filecount;

    for (uint64_t i = 0; i < filecount; i++)
    {
        uint16_t namelen = *((uint16_t *)(archive->address + offset)); offset += sizeof(uint16_t);
        const char *filename; CARString2(filename, outdir, (archive->address + offset));
        mkdirs(dirname(filename));
        FILE *fp = fopen(filename, "wb");

        if (!fp)
        {
            free(filename);
            CV3Exit(ERR_OPEN, retval, mapexit);
        }

        offset += namelen + 1;
        CAREntryVersion3 *entry = (CAREntryVersion3 *)(archive->address + offset); offset += sizeof(CAREntryVersion3);
        fwrite(archive->address + (entry->offset + header->data), 1, entry->size, fp);
        free(filename);
        fclose(fp);
    }

mapexit:
    munmap(archive->address, archive->size);
    free(archive);
    
statexit:
    free(stats);
    
fdexit:
    close(fd);
    
exitexit:
    return retval;
}

const char **CV3ListFiles(Path arch, size_t *count, int *error)
{
    const char **list = NULL;
    bool flipendian = false;

    int fd = open(arch, O_RDONLY);
    if (fd < 0) CV3RExit(ERR_OPEN, error, exitexit);

    struct stat *stats = malloc(sizeof(struct stat));
    if (stat(arch, stats)) CV3RExit(ERR_STAT, error, fdexit);

    MemoryBlock *archive = malloc(sizeof(MemoryBlock));
    archive->address = mmap(NULL, stats->st_size, PROT_READ, MAP_SHARED | MAP_FILE, fd, 0);
    if (((intptr_t)archive->address) < 0) CV3RExit(ERR_MMAP, error, statexit);
    archive->size = stats->st_size;

    CARHeaderVersion3 *header = (CARHeaderVersion3 *)archive->address;
    if (!(header->magic == CAR_MAGIC_v3 || header->magic == CAR_CIGAM_v3)) CV3RExit(ERR_ARCHIVE, error, mapexit);
    if (memcmp(header->version, version, sizeof(Version))) CV3RExit(ERR_ARCHIVE, error, mapexit);
    if (header->magic == CAR_CIGAM_v3) flipendian = true;

    list = calloc(header->filecount, sizeof(const char *));
    uint64_t offset = sizeof(CARHeaderVersion3);
    bool utf32 = header->flags | kCV3UTF32;
    bool utf16 = header->flags | kCV3UTF16;
    size_t filecount = header->filecount;
    if (count) (*count) = filecount;

    for (uint64_t i = 0; i < filecount; i++)
    {
        uint16_t strlen = *((uint16_t *)(archive->address + offset)); offset += sizeof(uint16_t);
        list[i] = calloc(strlen + 1, sizeof(char));
        memcpy(list[i], (archive->address + offset), strlen + 1); offset += strlen + 1;
        offset += sizeof(CAREntryVersion3);
    }

mapexit:
    munmap(archive->address, archive->size);
    free(archive);

statexit:
    free(stats);

fdexit:
    close(fd);

exitexit:
    return list;
}
