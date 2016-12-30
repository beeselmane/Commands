#include <sys/fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define bsize (1 << 28)

bool testEQ(const char *path1, const char *path2)
{
    struct stat stats[2];
    if (stat(path1, &stats[0])) return false;
    if (stat(path2, &stats[1])) return false;
    if (stats[0].st_size != stats[1].st_size) return false;
    if (stats[0].st_size == 0) return true;

    int fds[2];
    fds[0] = open(path1, O_RDONLY);
    fds[1] = open(path2, O_RDONLY);

    uint8_t *buffers[2];
    buffers[0] = malloc(bsize);
    buffers[1] = malloc(bsize);

    ssize_t rsize[2];
    rsize[0] = -1;
    rsize[1] = -1;

    bool rval = false;

    do {
        rsize[0] = read(fds[0], buffers[0], bsize);
        rsize[1] = read(fds[1], buffers[1], bsize);

        if (rsize[0] != rsize[1]) goto exit;
        if (rsize[0] <= 0) goto exit;

        if (memcmp(buffers[0], buffers[1], rsize[0])) goto exit;
    } while (rsize[0] == bsize);

    rval = true;

exit:
    free(buffers[0]);
    free(buffers[1]);

    close(fds[0]);
    close(fds[1]);

    return rval;
}
