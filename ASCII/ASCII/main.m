#import <Foundation/Foundation.h>

#define OUTFILE "/home/developer/home/ASCII.txt"

int main(int argc, const char *const *argv)
{
    @autoreleasepool
    {
        FILE *fp = fopen(OUTFILE, "w");
        for (uint8_t i = 0; i < 0x80; i++) fputc(i, fp);
        fclose(fp);
    }

    return(0);
}
