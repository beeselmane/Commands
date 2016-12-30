#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDONLY);
    struct stat stats;
    stat(argv[1], &stats);
    uint8_t *buffer = malloc(stats.st_size);
    read(fd, buffer, stats.st_size);
    close(fd);

    for (int i = 0; i < stats.st_size; i++)
    {
        printf("0x%02X, ", buffer[i]);
    }

    free(buffer);
    return 0;
}
