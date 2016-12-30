#include <unistd.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *const *argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "Usage: %s [files]\n", argv[0]);
        exit(1);
    }

    for (int i = 1; i < argc; i++)
    {
        if (rename(argv[i], basename((char *)argv[i])))
            fprintf(stderr, "Error moving file '%s'\n", argv[i]);
    }

    exit(0);
}
