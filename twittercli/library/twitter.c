#include "tw_internal.h"
#include "twitter.h"

// Module Initializer
__attribute__((constructor)) static void _TWInit(int argc, const char *const *argv)
{
    debug(printf("*** BEGIN _TWInit(); ***\n"));
    TWKeyStoreInitialize();
    debug(printf("*** END _TWInit(); ***\n"));
}
