#include <stdio.h>
#include "car.h"

int main(int argc, const char *const *argv)
{
    printf("%d\n", CArchiveCreateWithVersion(kCArchiveVersion_4_B, argv[1], argv[2]));
    return 0;
}
