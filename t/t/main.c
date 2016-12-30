#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>

#define fore(x) for (int i = 0; i < x; i++)
#define ksize   32

typedef struct {
    uint32_t key;
    uint8_t  __d0[22];
    uint32_t datasize;
    uint8_t  __d1[10];
    uint8_t  cmd;
    uint32_t __d2;
    uint8_t  data[32];
} AppleSMCBuffer_t;

static uint32_t keys[2] = {'OSK0', 'OSK1'};

static IOReturn readkeys(uint8_t data32[2][ksize])
{
    io_connect_t port = (io_connect_t)0;
    io_service_t service = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleSMC"));
    if (!service) return kIOReturnError;

    kern_return_t sstatus = IOServiceOpen(service, mach_task_self(), 0, &port);
    if (sstatus != KERN_SUCCESS) return kIOReturnError;
    IOObjectRelease(service);

    AppleSMCBuffer_t is[2] = {
        {keys[0], {0}, ksize, {0}, 5, },
        {keys[1], {0}, ksize, {0}, 5, }
    };

    AppleSMCBuffer_t os[2] = {
        {0}, {0}
    };

    fore(2)
    {
        size_t osc = sizeof(os[i]);
        IOReturn status = IOConnectCallStructMethod(port, 2, &is[i], sizeof(is[i]), &os[i], &osc);
        if (status != kIOReturnSuccess) return status;
        memcpy(data32[i], os[i].data, ksize);
    }

    return kIOReturnSuccess;
}

void printkey(uint8_t key[ksize])
{
    putchar('{');
    fore(ksize - 1) printf("0x%02x, ", key[i]);
    printf("0x%02x}\n", key[ksize - 1]);

    fore(ksize) putchar(key[i]);
    putchar('\n');
}

int main(int argc, const char *const *argv)
{
    uint8_t data[2][ksize];
    memset(data[0], 0, ksize);
    memset(data[1], 1, ksize);

    IOReturn r = readkeys(data);
    if (r != kIOReturnSuccess) exit(r);

    printkey(data[0]);
    printkey(data[1]);

    exit(EXIT_SUCCESS);
}

//ourhardworkbythesewordsguardedpleasedontsteal(c)AppleComputerInc
