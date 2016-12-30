#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int64_t key[3][3] = {
    {15, 13, 27},
    {21,  5,  8},
    { 4, 20,  2}
};

static int64_t inverse[3][3] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}
};

uint64_t determinant(int64_t matrix[3][3], int64_t modulo)
{
    int64_t d0, d1, d2;

    d0 = (matrix[1][1] * matrix[2][2]) -
         (matrix[1][2] * matrix[2][1]);

    d1 = (matrix[1][0] * matrix[2][2]) -
         (matrix[1][2] * matrix[2][0]);

    d2 = (matrix[1][0] * matrix[2][1]) -
         (matrix[1][1] * matrix[2][0]);

    d0 *= matrix[0][0];
    d1 *= matrix[0][1];
    d2 *= matrix[0][2];

    int64_t d = d0 - d1 + d2;
    return (d % modulo);
}

int64_t inverse_mod(int64_t n, int64_t m)
{
    for (int i = 0; i < n; i++)
    {
        if (((n * i) % m) == 1)
            return i;
    }

    return -1;
}

void invert(int64_t matrix[3][3], int64_t modulo)
{
    inverse[0][0] = (matrix[1][1] * matrix[2][2]) -
                    (matrix[1][2] * matrix[2][1]);

    inverse[0][1] = (matrix[0][2] * matrix[2][1]) -
                    (matrix[0][1] * matrix[2][2]);

    inverse[0][2] = (matrix[0][1] * matrix[1][2]) -
                    (matrix[0][2] * matrix[1][1]);

    inverse[1][0] = (matrix[1][2] * matrix[2][0]) -
                    (matrix[1][0] * matrix[2][2]);

    inverse[1][1] = (matrix[0][0] * matrix[2][2]) -
                    (matrix[0][2] * matrix[2][0]);

    inverse[1][2] = (matrix[0][2] * matrix[1][0]) -
                    (matrix[0][0] * matrix[1][2]);

    inverse[2][0] = (matrix[1][0] * matrix[2][1]) -
                    (matrix[1][1] * matrix[2][0]);

    inverse[2][1] = (matrix[0][1] * matrix[2][0]) -
                    (matrix[0][0] * matrix[2][1]);

    inverse[2][2] = (matrix[0][0] * matrix[1][1]) -
                    (matrix[0][1] * matrix[1][2]);

    int64_t det = determinant(matrix, modulo);
    int64_t det_inv = inverse_mod(det, modulo);

    if (det_inv < 0)
    {
        fprintf(stderr, "Error: Determinant has no inverse in Number System!\n");
        return;
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            while (inverse[i][j] < 0)
                inverse[i][j] += modulo;

            inverse[i][j] *= det_inv;
            inverse[i][j] %= 29;

            printf("%llu", inverse[i][j]);

            if (j == 2) printf("\n");
            else printf(", ");
        }
    }
}

int main(int argc, const char *const *argv)
{
    invert(key, 29);

    return(0);
}
