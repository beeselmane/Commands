#include "crv1.h"

#undef BIG_ENDIAN
#define BIG_ENDIAN 0

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// |  CAR v1 Utilities                                                                 | //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

CRv1LinkedList CRv1LLMake()
{
    CRv1LinkedList list = malloc(sizeof(struct __CRv1LinkedList));
    list->first = NULL;
    list->last = NULL;
    list->length = 0;
    return(list);
}

void CRv1LLKill(CRv1LinkedList list)
{
    CRv1LinkedListEntry entry = list->first, next = NULL;
    
    while (entry)
    {
        next = entry->next;
        free(entry);
        entry = next;
    }
    
    free(list);
}

bool CRv1LLHas(CRv1LinkedList list, CString value)
{
    CRv1LinkedListEntry entry = list->first;
    while (entry) if (!strcmp(entry->value, value)) return(true);
    return(false);
}

void CRv1LLAdd(CRv1LinkedList list, CString label)
{
    CRv1LinkedListEntry entry = malloc(sizeof(struct __CRv1LLENT));
    entry->value = label;
    entry->next = NULL;
    if (list->last) list->last->next = entry;
    else list->first = entry;
    list->last = entry;
    list->length++;
}

void CRv1LLTake(CRv1LinkedList list, CString value)
{
    CRv1LinkedListEntry entry = list->first, last = NULL;
    
    while (entry)
    {
        if (!strcmp(entry->value, value))
        {
            if (last == NULL) {
                list->first = entry->next;
                free(entry);
            } else {
                if (entry->next == NULL) list->last = last;
                last->next = entry->next;
                free(entry);
            }
        }
        
        entry = entry->next;
    }
}

CAR_PRIVATE Size CRv1AddDirectory(CString directory, CRv1LinkedList list, CString *error, bool verbose)
{
    if (access(directory, R_OK | X_OK))
    {
        (*error) = "Permissions";
        if (verbose) fprintf(stderr, "*** Error: Could not access directory: %s ***\n*** Stop. ***\n", directory);
        return(false);
    }
    
    DIR *dir;
    Size ret = 0;
    struct dirent *entry;
    dir = opendir(directory);
    Size directroyNameSize = strlen(directory);
    
    if (dir) {
        while ((entry = readdir(dir)))
        {
            if ((entry->d_name[0] == '.' && entry->d_name[1] == 0) || (entry->d_name[0] == '.' && entry->d_name[1] == '.' && entry->d_name[2] == 0)) continue;
            
            Size fullSize = directroyNameSize + strlen(entry->d_name) + 1;
            CString fullName = malloc((sizeof(char) * fullSize) + 1);
            sprintf(fullName, "%s/%s", directory, entry->d_name);
            
            struct stat stats;
            stat(fullName, &stats);
            
            if (stats.st_mode & S_IFDIR) {
                Size newDir = CRv1AddDirectory(fullName, list, error, verbose);
                if (newDir == -1) return(-1);
                ret += newDir;
            } else if (stats.st_mode & S_IFREG) {
                if (access(fullName, R_OK))
                {
                    (*error) = "Permissions";
                    if (verbose) fprintf(stderr, "*** Error: Could not access file: %s ***\n*** Stop. ***\n", fullName);
                    return(-1);
                }
                
                if (verbose) printf("L %s\n", fullName);
                CRv1LLAdd(list, fullName);
                list->last->fileSize = stats.st_size;
                ret += stats.st_size;
            } else {
                (*error) = "File Type";
                if (verbose) fprintf(stderr, "*** Error: Invalid File Type 0x%X for file %s ***\n*** Stop. ***\n", stats.st_mode, fullName);
                return(-1);
            }
        }
        
        closedir(dir);
    } else {
        (*error) = "Memory";
        if (verbose) fprintf(stderr, "*** Error: Could not open directory %s ***\n*** Stop. ***\n", directory);
        return(-1);
    }
    
    return(ret);
}

CAR_PRIVATE UInt16 CRv1WriteString(FILE *fp, CString string)
{
    UInt16 length = 0;
    while ((*string)) fputc((*string++), fp), length++;
    fputc(0, fp);
    return(++length);
}

CAR_PRIVATE UInt16 CRv1ReadString(FILE *fp, CString string)
{
    UInt16 length = 0;
    while (((*string) = fgetc(fp))) string++, length++;
    return(++length);
}

CAR_PRIVATE Size CRv1GetFileNameSizes(CRv1LinkedList list, Size offset)
{
    CRv1LinkedListEntry entry = list->first;
    Size size = 0;
    
    while (entry)
    {
        size += strlen(entry->value + offset) + 1;
        entry = entry->next;
    }
    
    return(size);
}

// Basic CRC-32 Calculator
CAR_PRIVATE UInt32 CRv1GetCRC32(unsigned char *message, Size size)
{
    static UInt32 table[256];
    UInt32 crc, byte;
    
    if (table[1] == 0)
    {
        for (byte = 0; byte <= 255; byte++)
        {
            crc = byte;
            
            for (int j = 7; j >= 0; j--)
            {
                UInt32 mask = -(crc & 1);
                crc = (crc >> 1) ^ (0xEDB88320 & mask);
            }
            
            table[byte] = crc;
        }
    }
    
    crc = 0xFFFFFFFF;
    
    for (int i = 0; i < size; i++)
    {
        byte = message[i];
        crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF];
    }
    
    return ~crc;
}

CAR_PRIVATE void CRv1MakeFullPath(CString path)
{
    if (path[0] == 0) return;
    
    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%s", path);
    Size length = strlen(tmp);
    
    if (tmp[length - 1] == '/') tmp[length - 1] = 0;
    
    for (char *p = tmp + 1; (*p); p++)
    {
        if ((*p) == '/')
        {
            (*p) = 0;
            mkdir(tmp, S_IRWXU);
            (*p) = '/';
        }
    }
    
    mkdir(tmp, S_IRWXU);
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
// |  CAR v1 Functions                                                                 | //
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

/*
 * Generate CAR File with contents of `rootdir`
 * Adds all files from all subdirectories preserving heiarchy
 *
 * Steps:
 *   1: Check if the output file is accessible
 *   2: Generate list of files to create archive with
 *        * Fails if access is denied to any file or directory
 *   3: Generate Table of Contents and dump file data into archive
 */
bool CRv1CreateCAR(CString rootdir, CString car, CString *error, bool verbose, bool doChecksum)
{
    if (verbose) printf("1. Checking Access...\n");
    FILE *fp;

    if (!doChecksum) fp = fopen(car, "wb");
    else             fp = fopen(car, "rb+");

    if (!fp)
    {
        (*error) = "Could not open output file!";
        if (verbose) fprintf(stderr, "*** Error: Could not open output file ***\n*** Stop. ***\n");
        return(false);
    }
    
    if (verbose) printf("Done.\n");
    if (verbose) printf("2. Listing Files...\n");

    CRv1LinkedList list = CRv1LLMake();
    Size extsize = CRv1AddDirectory(rootdir, list, error, verbose);
    
    if (extsize == -1)
    {
        fclose(fp);
        unlink(car);
        return(false);
    }
    
    if (verbose) printf("3. Creating Archive...\n");
    
    Offset shift = strlen(rootdir);
    CRv1LinkedListEntry entry = list->first;
    Size nameSizes = CRv1GetFileNameSizes(list, shift);

    char magic[3] = {'C', 'A', 'R'};
    UInt8 version = 1;
    UInt8 flags = 0;
    if (doChecksum) flags |= (1 << 6);
    if (BIG_ENDIAN) flags |= (1 << 7);
    Size filecount = list->length;
    Size fullsize = extsize + HEADER_SIZE + (ENTRY_SIZE * list->length) + nameSizes;
    UInt32 checksum = 0;

    fwrite(magic,       sizeof(char),   3, fp);
    fwrite(&version,    sizeof(UInt8),  1, fp);
    fwrite(&flags,      sizeof(UInt8),  1, fp);
    fwrite(&filecount,  sizeof(UInt64), 1, fp);
    fwrite(&fullsize,   sizeof(Size),   1, fp);
    fwrite(&extsize,    sizeof(Size),   1, fp);
    Offset checkoff = ftell(fp);
    fwrite(&checksum,   sizeof(UInt32), 1, fp);

    Offset currentOffset = ftell(fp);
    Offset dataPtr = HEADER_SIZE + (ENTRY_SIZE * list->length) + nameSizes;

    for (int i = 0; i < list->length; i++)
    {
        if (verbose) printf("A %s\n", ((entry->value) + shift));
        fseek(fp, currentOffset, SEEK_SET);
        
        FILE *f = fopen(entry->value, "rb");
        MemoryAddress fileData = malloc(entry->fileSize);
        fread(fileData, 1, entry->fileSize, f);
        fclose(f);
        
        CRv1CARFileEntryRef fileEntry = malloc(sizeof(struct __CARFileEntryV1));
        fileEntry->name = ((entry->value) + shift);
        fileEntry->type = 0;
        fileEntry->data = dataPtr;
        fileEntry->size = entry->fileSize;
        fileEntry->checksum = 0;
        fileEntry->mdSize   = 0;
        
        UInt16 strLen = CRv1WriteString(fp, fileEntry->name);
        //fwrite(&fileEntry->type,     sizeof(UInt8),  1, fp);
        fwrite(&fileEntry->data,     sizeof(Offset), 1, fp);
        fwrite(&fileEntry->size,     sizeof(Size),   1, fp);
        fwrite(&fileEntry->checksum, sizeof(UInt16), 1, fp);
        //fwrite(&fileEntry->mdSize,   sizeof(UInt8),  1, fp);
        
        fseek(fp, dataPtr, SEEK_SET);
        fwrite(fileData, 1, entry->fileSize, fp);
        free(fileData);
        
        currentOffset += ENTRY_SIZE + strLen;
        dataPtr += entry->fileSize;
        entry = entry->next;
    }

    if (doChecksum)
    {
        Size readSize = currentOffset;
        rewind(fp);
        MemoryAddress buffer = malloc(readSize);
        fread(buffer, 1, readSize, fp);
        checksum = CRv1GetCRC32(buffer, readSize);
        fseek(fp, checkoff, SEEK_SET);
        fwrite(&checksum, sizeof(UInt32), 1, fp);
        rewind(fp);
        fflush(fp);
    }

    fclose(fp);
    return(true);
}

CString *CRv1ListFiles(CString car, UInt64 *count, CRv1Flags flags)
{
    FILE *fp = fopen(car, "rb");
    if (!fp) return(false);
    
    UInt8 *header = malloc(HEADER_SIZE);
    fread(header, HEADER_SIZE, 1, fp);
    UInt64 fcount = (*(UInt64 *)(header + 13));
    CString *ret = malloc(sizeof(CString) * fcount);
    if (count) (*count) = fcount;
    
    for (int i = 0; i < fcount; i++)
    {
        char name[1024];
        UInt16 length = CRv1ReadString(fp, name);
        fseek(fp, ENTRY_SIZE, SEEK_CUR);
        ret[i] = malloc(sizeof(char) * length);
        strncpy(ret[i], name, length);
    }

    return(ret);
}

bool CRv1GrabFile(CString car, CString file, CString out, CRv1Flags flags)
{
    FILE *fp = fopen(car, "rb");
    if (!fp) return(false);

    UInt8 *header = malloc(HEADER_SIZE);
    fread(header, HEADER_SIZE, 1, fp);
    UInt64 fcount = (*(UInt64 *)(header + 13));
    
    for (int i = 0; i < fcount; i++)
    {
        char name[1024];
        UInt16 length = CRv1ReadString(fp, name) - 1;
        
        if (!strncmp(name, file, length))
        {
            FILE *f = fopen(out, "wb");

            Offset data; Size size;
            UInt16 checksum;
            
            fread(&data, sizeof(Offset), 1, fp);
            fread(&size, sizeof(Size),   1, fp);
            fread(&checksum, sizeof(UInt16), 1, fp);
            
            if (!f)
            {
                fclose(fp);
                return(false);
            }
            
            MemoryAddress raw = malloc(size);
            
            fseek(fp, data, SEEK_SET);
            fread(raw, 1, size, fp);
            fwrite(raw, 1, size, f);
            
            fclose(fp);
            fclose(f);
            return(true);
        }
        
        fseek(fp, ENTRY_SIZE, SEEK_CUR);
    }
    
    fclose(fp);
    return(false);
}

bool CRv1ExtractCar(CString car, CString outdir, CRv1Flags flags)
{
    if (access(outdir, W_OK)) if (mkdir(outdir, S_IRWXU | S_IRWXG)) return(false);

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    chdir(outdir);
    
    UInt64 count = 0;
    CString *files = CRv1ListFiles(car, &count, flags);
    
    if (!files || count == 0) return(false);
    
    for (int i = 0; i < count; i++)
    {
        char file[1024];
        sprintf(file, "%s/%s", outdir, files[i]);
        printf("X %s\n", files[i]);
        CRv1MakeFullPath(dirname(files[i]) + 1);
        if (!CRv1GrabFile(car, files[i], file, flags)) return(false);
    }
    
    chdir(cwd);
    return(true);
}