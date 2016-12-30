#include <stdio.h>

#define T 100000000

// FizzBuzz is Q for whatever reason
#define L "\n"
#define N "%d"       L
#define F "Fizz"     L
#define B "Buzz"     L
#define Q "FizzBuzz" L

static const char *fizzbuzz[15] = {
    Q, N, N, F, N,
    B, F, N, N, F,
    B, N, F, N, N
};

int array(void)
{
    for (int i = 1; i <= T; i++) printf(fizzbuzz[i % 15], i);
    return(0);
}

int stand(void)
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
