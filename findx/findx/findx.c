#include "commoncrypto.h"

#include <sys/stat.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

uint64_t *checksum(const char *path)
{
    uint64_t *sum = malloc(sizeof(uint64_t) * 2);
    Digest_File(kCCDigestMD5, path, (char *)sum);
    return sum;
}

bool sumequal(uint64_t *s1, uint64_t *s2)
{
    return (s1[0] == s2[0] && s1[1] == s2[1]);
}

void findall(const char *path, uint64_t size, uint64_t *digest)
{
    DIR *dir = opendir(path);
    if (!dir) return;

    struct dirent *filename;

    while ((filename = readdir(dir)))
    {
        if (filename->d_namlen == 2 && filename->d_name[0] == '.' && filename->d_name[1] == '.') continue;
        if (filename->d_namlen == 1 && filename->d_name[0] == '.') continue;

        char *fullpath = NULL;
        asprintf(&fullpath, "%s/%s", path, filename->d_name);

        struct stat stats;
        lstat(fullpath, &stats);

        if (S_ISREG(stats.st_mode)) {
            if (stats.st_size == size)
            {
                uint64_t *mdsum = checksum(fullpath);

                if (sumequal(mdsum, digest))
                {
                    // We found one :D
                    printf("Found %s\n", fullpath);
                }

                free(mdsum);
            }
        } else if (S_ISDIR(stats.st_mode)) {
            findall(fullpath, size, digest);
        }
    }

    closedir(dir);
}

// Usage: findx <dirname> <file>
int main(int argc, char *const *argv)
{
    if (argc != 3) exit(EXIT_FAILURE);

    const char *rootdir = argv[1];
    const char *model = argv[2];

    struct stat stats;
    stat(model, &stats);
    uint64_t *sum = checksum(model);

    printf("Looking for matches of %s...\n", model);
    findall(rootdir, stats.st_size, sum);
    free(sum);

    exit(EXIT_SUCCESS);
}
