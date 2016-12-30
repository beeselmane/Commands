#include <libcar.h>
#include <stdlib.h>
#include <stdio.h>

// -c, create
// -r, append
// -t, list
// -u, replace
// -x, extract

// c, r, @fname: Open `fname` and append all files to current archive
// -b blocksize: Specify blocksize for tapes
// -C directory: Change to directory before adding/extracting files
// c, r, --check-links: Warn unless all links are working
// x, --chroot: chroot to current directory (after -C) before extracting
// --exclude pattern: Ignore files that match pattern
// c, r, u, --format format: Specify Archive format (GNU tar supports cpio, pax, shar, and ustar)
// -f file: read/write archive to file. `-` means stdin
// c, r, -H: Follow named symbolic links (From command line args). Don't archive these link
// -I: Same as -T
// --include pattern: Ignore all files that don't match pattern
// c, -j: Run archive through `bzip2`. Ignored unless in -c mode.
// x, -k: Do not overwrite existing files (Override note 2)
// x, --keep-newer-files: Do not overwrite unless archived version is newer
// c, r, -L: Follow all symbolic links. Don't archive any links.
// -l: Same as --check-links
// x, -m: Don't set modification time from archive
// c, r, u, -n: Don't recurse into directories
// c, r, u, --newer date: Only process files newer than date
// c, r, u, --newer-mtime date: Only process files modified more recently than date
// c, r, u, --newer-than file: Only process files newer than file
// c, r, u, --newer-mtime-than file: Only process files modified more recently than file
// c, r, --nodump: Honor nodump file flag
// --null: ???
// x, --numeric-owner: Ignore symbolic user/group names. Only respect id numbers
// x, t, -O: write to stdout in extract mode. Write to stderr in list mode.
// x, -o: Ignore ACLs and owner information. Use current user's permissions
// c, r, u, -o: --format ustar
// c, r, u, --one-file-system: Archive only files on one filesystem. Exclude external disks/mount points
// --options options: ???
// -P: preserve exact pathnames
// -p: preserve file permissions
// x, t, -q, --fast-read: Exit as soon as specified filename or pattern is matched
// x, -S: Extract as sparse files. Search disk
// x, --strip-components count: Remove the specified number of path components
// -s pattern: Modify according to pattern
// -T filename: Read list of files from filename (-C by itself changes current directory)
// x, -U: Unlink Files before overwriting them
// x, t, --use-compress-program program: pipe input through program
// -v: Produce Verbose output
// --version: Print versions
// -w: Ask for confirmation on all actions
// -X filename: Read a list of exclusion patterns from filename
// c, -y: Pipe archive through bzip2
// c, -z: Compress with gzip
// c, -Z: Compress with `compress` command-line utility

// Note 1: If extracting 2 files with the same name, prior versions will be overwritten
// Note 2: Files are added in specified order.

static const char *name;

__attribute__((noreturn, always_inline)) static void usage(void)
{
    fprintf(stderr, "Usage:\n"                                      );
    fprintf(stderr, "\t%s -c Archive.car directory\n",          name);
    fprintf(stderr, "\t%s -x Archive.car directory\n",          name);
    fprintf(stderr, "\t%s -r Archive.car /filename [output]\n", name);
    fprintf(stderr, "\t%s -l Archive.car",                      name);
    fprintf(stderr, "\n"                                            );
    fprintf(stderr, "About:\n"                                      );
    fprintf(stderr, "\t-c: Create Archive from directory\n"         );
    fprintf(stderr, "\t-x: Extract Archive into directory\n"        );
    fprintf(stderr, "\t-r: Extract file from Archive\n"             );
    fprintf(stderr, "\t-l: List Files in Archive\n"                 );

    exit(1);
}

#define HOME      "/Users/developer/"
#define DESKTOP   HOME "Desktop/"
#define DOCUMENTS HOME "Documents/"

#define ARCHIVE DESKTOP "Archive.cv3"
#define ROOTDIR DESKTOP "MachO"

#define ARCHFILE "/machoview/otool_disasm/symbol.c"
#define OUTFILE  DESKTOP "symbol.c"
#define OUTDIR   DESKTOP "mo_ext"

int main(int argc, const char *const *argv)
{
    name = argv[0];

#if 0
    if (argc != 1) usage();

    //int rval = CV3CreateArchive(ARCHIVE, ROOTDIR, 0);

    //size_t count = 0; int rval = 0;
    //const char **list = CV3ListFiles(ARCHIVE, &count, &rval);

    /*int rval = 0;
    FileBuffer *buffer = CV3GetFile(ARCHIVE, ARCHFILE, &rval);
    if (rval != ERR_NONE || !buffer) goto errcase;
    FILE *fp = fopen(OUTFILE, "wb");
    fwrite(buffer->address, 1, buffer->size, fp);
    fclose(fp);

    buffer->free(buffer);*/

    int rval = CV3ExtractArchive(ARCHIVE, OUTDIR);

errcase:
    switch (rval)
    {
        case ERR_NONE: break;
        //case ERR_NONE: {
        //    printf("Read %zu files from %s:\n", count, ARCHIVE);
        //    for (size_t i = 0; i < count; i++) printf("%zu: %s\n", (i + 1), list[i]);
        //} break;
        case ERR_ARCHIVE: {
            printf("Malformed Archive: %s\n", ARCHIVE);
        } break;
        case ERR_ACCESS: perror("access"); break;
        case ERR_STAT:   perror("stat");   break;
        case ERR_OPEN:   perror("open");   break;
        case ERR_MMAP:   perror("mmap");   break;
        default: printf("?\n%d\n", rval);  break;
    }

    return(0);
#else
    if (argc < 2)           usage();
    if (argv[1][0] != '-')  usage();
    if (argv[1][2] != '\0') usage();

    FileBuffer *filebuffer = NULL;
    const char **filelist = NULL;
    int errval = ERR_NONE;
    size_t listsize = 0;

    switch (argv[1][1])
    {
        case 'c': {
            if (argc != 4) usage();
            errval = CV3CreateArchive(argv[2], argv[3], 0);
        } break;
        case 'x': {
            if (!(argc == 3 || argc == 4)) usage();
            errval = CV3ExtractArchive(argv[2], (argv[3]) ? argv[3] : ".");
        } break;
        case 'r': {
            if (!(argc == 4 || argc == 5)) usage();
            filebuffer = CV3GetFile(argv[2], argv[3], &errval);
        } break;
        case 'l': {
            if (argc != 3) usage();
            filelist = CV3ListFiles(argv[2], &listsize, &errval);
        } break;
        default: usage(); break;
    }

    switch (errval)
    {
        case ERR_NONE: break;
        case ERR_ARCHIVE: {
            printf("Malformed Archive: %s\n", ARCHIVE);
            exit(1);
        } break;
        case ERR_ACCESS: perror("access");   exit(1); break;
        case ERR_STAT:   perror("stat");     exit(1); break;
        case ERR_OPEN:   perror("open");     exit(1); break;
        case ERR_MMAP:   perror("mmap");     exit(1); break;
        default: printf("?\n%d\n", errval);  exit(1); break;
    }

    switch (argv[1][1])
    {
        case 'r': {
            const char *output;
            if (argv[4]) output = argv[4];
            else asprintf(&output, ".%s", argv[3]);
            FILE *fp = fopen(output, "wb");

            if (!fp)
            {
                fprintf(stderr, "Could not open output file for writing!\n");
                exit(1);
            }

            fwrite(filebuffer->address, 1, filebuffer->size, fp);
            filebuffer->free(filebuffer);
            fclose(fp);
        } break;
        case 'l': {
            for (size_t i = 0; i < listsize; i++) printf("%zu: %s\n", (i + 1), filelist[i]);
        } break;
    }

    exit(0);
#endif
}
