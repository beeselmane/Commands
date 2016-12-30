#include <CoreFoundation/CoreFoundation.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define barroff {0, 0, 0, 0, 0, 0, 0, 0}
#define nqueens bsize
#define bsize   8

#define fxord(i, s) ((s - 1) - (i))

static bool columns[bsize] = barroff;
static bool rows[bsize] = barroff;

// board[row][col] board[y][x]
// column = x, row = y
static bool board[bsize][bsize] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

static void printBoard(void);
static bool placeQueen(int px, int py);
static void reset();
static bool checkDiagXY(int d);
static bool checkDiagYX(int d);
static bool isValid();
static void printBoard(void);

static bool placeQueen(int px, int py)
{
    if (columns[px] || rows[py]) return false;
    board[py][px] = true;

    if (!checkDiagYX(px + py)) {
        board[py][px] = false;
        return false;
    } if (!checkDiagXY(fxord(px, bsize) + fxord(py, bsize))) {
        board[py][px] = false;
        return false;
    }

    return (columns[px] = rows[py] = true);
}

static void reset()
{
    memset(columns, 0, bsize);
    memset(rows, 0, bsize);
    memset(board, 0, (bsize * bsize));
}

static bool checkDiagXY(int d)
{
    if (d == 0 || d == ((bsize * 2) - 2)) return true;

    bool seenq = false;
    int x, y;
    
    if (d < bsize) {
        y = d;
        x = bsize - 1;
    } else {
        x = (d - bsize);
        y = bsize - 1;
    }
    
    for ( ; x >= 0; )
    {
        if (board[y][x])
        {
            if (seenq) {
                return false;
            } else {
                seenq = true;
            }
        }
        
        x++;
        y--;
    }

    return true;
}

static bool checkDiagYX(int d)
{
    if (d == 0 || d == ((bsize * 2) - 2)) return true;

    bool seenq = false;
    int x, y;
    
    if (d < bsize) {
        y = d;
        x = 0;
    } else {
        x = (d - bsize);
        y = bsize;
    }
    
    for ( ; y >= 0; )
    {
        if (board[y][x])
        {
            if (seenq) {
                return false;
            } else {
                seenq = true;
            }
        }
        
        y--;
        x++;
    }

    return true;
}

static bool isValid()
{
    for (int d = 0; d < (bsize * 2) - 1; d++) if (!checkDiagXY(d) || !checkDiagYX(d)) return false;
    return true;
}

static void mksol()
{
    do {
        reset();
        int soff = 0;

        for (int q = 0; q < nqueens; )
        {
            for (int y = 0; y < bsize; y++)
            {
                int lq = q;

                for (int x = 0; x < bsize; x++)
                {
                    if (!placeQueen((x + soff), y)) continue;
                    q++; break;
                }

                if (lq != q) break;
            }
        }

        soff++;
        printBoard();
        printf("\n");
    } while (!isValid());
}

static void printBoard(void)
{
    for (int i = 0; i < bsize; i++)
    {
        for (int j = 0; j < bsize; j++)
        {
            printf("%d", board[i][j]);
            if (j != (bsize - 1)) printf(", ");
        }

        putchar('\n');
    }
}

int main(int argc, const char *const *argv)
{
    printBoard();
    printf("\n");
    mksol();

    return(0);
}
