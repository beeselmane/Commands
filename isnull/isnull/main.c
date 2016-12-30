#include <sys/fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define bufsize (1 << 28)

void checkFile(const char *path)
{
    int fd = open(path, O_RDONLY);

    if (!fd)
    {
        fprintf(stderr, "%s: error\n", path);
        return;
    }

    uint8_t *nullbuffer = malloc(bufsize);
    uint8_t *readbuffer = malloc(bufsize);
    memset(nullbuffer, 0, bufsize);
    ssize_t rsize = -1;

    do {
        rsize = read(fd, readbuffer, bufsize);

        if (rsize < 0)
        {
            fprintf(stderr, "%s: error\n", path);
            free(readbuffer);
            free(nullbuffer);
            close(fd);

            return;
        }

        if (rsize == 0) break;

        if (memcmp(nullbuffer, readbuffer, rsize))
        {
            fprintf(stderr, "%s: no\n", path);
            free(readbuffer);
            free(nullbuffer);
            close(fd);

            return;
        }
    } while (rsize == bufsize);

    fprintf(stderr, "%s: yes\n", path);
    free(readbuffer);
    free(nullbuffer);
    close(fd);
}

int main(int argc, const char *const *argv)
{
    if (argc <= 1)
    {
        fprintf(stderr, "usage: %s file ...\n", argv[0]);
        exit(1);
    }

    for (int i = 1; i < argc; i++)
        checkFile(argv[i]);

    exit(0);
}
