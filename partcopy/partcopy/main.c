#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define bufsize (1 << 27)

static void usage(const char *name)
{
    fprintf(stderr, "Usage: %s <input file> <start offset + 1> <end offset + 1>\n", name);
    exit(1);
}

int main(int argc, const char *const *argv)
{
    if (argc != 4) usage(argv[0]);
    const char *input_filename = argv[1];

    char *startError = NULL, *endError = NULL;
    uint64_t startAddr = strtoull(argv[2], &startError, 10) - 1;
    uint64_t endAddr = strtoull(argv[3], &endError, 10) - 1;
    if (startError == argv[2] || endError == argv[3]) usage(argv[0]);
    fprintf(stderr, "Reading file '%s' from %llu to %llu...\n", input_filename, startAddr, endAddr);
    size_t amount = endAddr - startAddr;

    int fd = open(input_filename, O_RDONLY);
    off_t offset = lseek(fd, startAddr, SEEK_SET);

    if (offset != startAddr)
    {
        fprintf(stderr, "Seek Error\n");
        perror("lseek");
        exit(1);
    }

    uint8_t *buffer = malloc(bufsize);
    ssize_t amountLeft = amount;
    ssize_t toRead = bufsize;
    ssize_t rsize = -1;

    do {
        if (amountLeft < bufsize) toRead = amountLeft;
        rsize = read(fd, buffer, toRead);

        if (rsize < 0)
        {
            fprintf(stderr, "Read Error\n");
            perror("read");
            exit(1);
        }

        if (!rsize) break;
        write(STDOUT_FILENO, buffer, rsize);
        amountLeft -= rsize;
    } while (rsize == toRead);

    fprintf(stderr, "Read %lu of %lu bytes.\n", (amount - amountLeft), amount);
    free(buffer);
    close(fd);

    return(0);
}
