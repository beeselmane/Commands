#if 0

#include <stdio.h>

#define L "\n"
#define N "%d"       L
#define F "Fizz"     L
#define B "Buzz"     L
#define Q "FizzBuzz" L

#define T 1000
#define G stand

int G(void)
{
    for (int i = 1; i <= T; i++)
    {
        if (!(i % 3) && !(i % 5)) printf(Q);
        else if (!(i % 3)) printf(F);
        else if (!(i % 5)) printf(B);
        else printf(N, i);
    }

    return(0);
}

#endif /* 0 */
