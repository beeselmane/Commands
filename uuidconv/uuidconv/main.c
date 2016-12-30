#include <stdio.h>

void one(char *s)
{
    printf("((uuid){0x%c%c, 0x%c%c, 0x%c%c, 0x%c%c, "
                   "0x%c%c, 0x%c%c, 0x%c%c, 0x%c%c, "
                   "0x%c%c, 0x%c%c, 0x%c%c, 0x%c%c, "
                   "0x%c%c, 0x%c%c, 0x%c%c, 0x%c%c})\n",
                    s[0],   s[1],   s[2],   s[3],
                    s[4],   s[5],   s[6],   s[7],
                    s[9],   s[10],  s[11],  s[12],
                    s[14],  s[15],  s[16],  s[17],
                    s[19],  s[20],  s[21],  s[22],
                    s[24],  s[25],  s[26],  s[27],
                    s[28],  s[29],  s[30],  s[31],
                    s[32],  s[33],  s[34],  s[35]);
}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
        one(argv[i]);

    return 0;
}
