#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static uint64_t i1 = 0;
static uint64_t i2 = 41726498;
static uint64_t i3 = 637283918237498234;
static uint64_t i4 = 0xFFFFFFFFFFFFFFFF;

static uint64_t a1 = 0x0;
static uint64_t a2 = 0xABCDEFED;
static uint64_t a3 = 0xFC34;
static uint64_t a4 = 0xFEDCBABCDEFEDCBA;
static uint64_t a5 = 0x1234567890ABCDEF;

void printU(uint64_t number, const char *prefix, uint8_t base)
{
    static const char *characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (prefix) printf("%s", prefix);

    if (!number)
    {
        putchar('0');
        return;
    }

    char string[32] = {0};
    int i = 0;

    do {
        uint64_t divisor = number % base;
        string[i++] = characters[divisor];
        number /= base;
    } while (number);

    for (int8_t j = i; j >= 0; j--)
    {
        if (string[j])
            putchar(string[j]);
    }
}

void printDecimal(uint64_t number)
{
    char string[20] = {0};
    int i = 0;

    do {
        string[i++] = (number % 10) + '0';
        number /= 10;
    } while (number > 0);

    for (int8_t j = i; j >= 0; j--)
    {
        if (string[j])
            putchar(string[j]);
    }
}

void printAddress(void *address)
{
    static const char *characters = "01236789ABCDEF";
    uint64_t number = (uint64_t)address;
    char string[16] = {0};
    printf("0x");
    int i = 0;

    do {
        uint64_t divisor = number % 16;
        string[i++] = characters[divisor];
        number /= 16;
    } while (number > 0);

    for (int8_t j = i; j >= 0; j--)
    {
        if (string[j])
            putchar(string[j]);
    }
}

int main(void)
{
    printU(i1, NULL, 10);
    putchar('\n');
    printU(i2, NULL, 10);
    putchar('\n');
    printU(i3, NULL, 10);
    putchar('\n');
    printU(i4, NULL, 10);
    putchar('\n');

    printU(a1, "0x", 16);
    putchar('\n');
    printU(a2, "0x", 16);
    putchar('\n');
    printU(a3, "0x", 16);
    putchar('\n');
    printU(a4, "0x", 16);
    putchar('\n');
    printU(a5, "0x", 16);
    putchar('\n');

    return 0;
}
