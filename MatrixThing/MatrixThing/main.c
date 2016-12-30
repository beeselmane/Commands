#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static uint8_t keyMatrix[3][3] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}
};

static char *text;
static char *result;

static uint8_t *resultMatrix[3] = {0, 0, 0};
static uint8_t *textMatrix[3] = {0, 0, 0};
static size_t textMatrixSize = 0;

uint8_t process(char c)
{
    if (c == '?' || c == '!') return 28;
    if (c == '_') return 26;
    if (c == '.') return 27;
    
    return ((c | 0x20) - 'a');
}

char unprocess(uint8_t n)
{
    if (n == 28) return '!';
    if (n == 27) return '.';
    if (n == 26) return '_';
    
    return ((n + 'a') | 0x20);
}

void parseText(const char *inputText)
{
    size_t length = strlen(inputText);
    textMatrixSize = length / 3;
    size_t lengthMod3 = length % 3;
    if (lengthMod3) textMatrixSize++;
    
    textMatrix[0] = malloc(textMatrixSize);
    textMatrix[1] = malloc(textMatrixSize);
    textMatrix[2] = malloc(textMatrixSize);
    
    for (size_t i = 0; i < length; i += 3)
    {
        size_t tm_i = i / 3;
        
        textMatrix[0][tm_i] = process(inputText[i + 0]);
        textMatrix[1][tm_i] = process(inputText[i + 1]);
        textMatrix[2][tm_i] = process(inputText[i + 2]);
    }
    
    switch (lengthMod3)
    {
        case 1: textMatrix[1][textMatrixSize - 1] = process('_');
        case 2: textMatrix[2][textMatrixSize - 1] = process('_');
        default: break;
    }
    
    text = (char *)inputText;
}

void parseKey(const char *keyText)
{
    if (strncmp(keyText, "random", strlen(keyText))) {
        if (strlen(keyText) != 28)
        {
            fprintf(stderr, "Invalid Key Text!\n");
            exit(1);
        }
        
        uint8_t numbers[9];
        char number[3];
        number[2] = 0;
        keyText++;
        
        for (int i = 0; i < 26; i += 3)
        {
            number[0] = keyText[i];
            number[1] = keyText[i + 1];
            
            numbers[i / 3] = atoi(number);
        }
        
        for (int i = 0; i < 9; i += 3)
        {
            keyMatrix[i / 3][0] = numbers[i + 0];
            keyMatrix[i / 3][1] = numbers[i + 1];
            keyMatrix[i / 3][2] = numbers[i + 2];
        }
    } else {
        for (size_t i = 0; i < 3; i++)
            for (size_t j = 0; j < 3; j++)
                keyMatrix[i][j] = rand() % 29;
    }
}

void dumpMessage()
{
    for (size_t i = 0; i < 3; i++)
    {
        putchar('{');
        
        for (size_t j = 0; j < textMatrixSize; j++)
        {
            printf("%c", unprocess(textMatrix[i][j]));
            
            if (j == (textMatrixSize - 1))  printf("}\n");
            else                            printf(", ");
        }
    }
}

void dumpMessageRaw()
{
    for (size_t i = 0; i < 3; i++)
    {
        putchar('{');
        
        for (size_t j = 0; j < textMatrixSize; j++)
        {
            printf("%02u", textMatrix[i][j]);
            
            if (j == (textMatrixSize - 1))  printf("}\n");
            else                            printf(", ");
        }
    }
}

void dumpResult()
{
    for (size_t i = 0; i < 3; i++)
    {
        putchar('{');
        
        for (size_t j = 0; j < textMatrixSize; j++)
        {
            printf("%c", unprocess(resultMatrix[i][j]));
            
            if (j == (textMatrixSize - 1))  printf("}\n");
            else                            printf(", ");
        }
    }
}

void dumpResultRaw()
{
    for (size_t i = 0; i < 3; i++)
    {
        putchar('{');
        
        for (size_t j = 0; j < textMatrixSize; j++)
        {
            printf("%02u", resultMatrix[i][j]);
            
            if (j == (textMatrixSize - 1))  printf("}\n");
            else                            printf(", ");
        }
    }
}

void dumpKey()
{
    for (size_t i = 0; i < 3; i++)
        printf("{%02u, %02u, %02u}\n", keyMatrix[i][0], keyMatrix[i][1], keyMatrix[i][2]);
}

void runEncrypt()
{
    resultMatrix[0] = malloc(textMatrixSize);
    resultMatrix[1] = malloc(textMatrixSize);
    resultMatrix[2] = malloc(textMatrixSize);
    
    for (int i = 0; i < textMatrixSize; i++)
    {
        for (int r = 0; r < 3; r++)
        {
            uint8_t n1 = keyMatrix[r][0] * textMatrix[0][i];
            uint8_t n2 = keyMatrix[r][1] * textMatrix[1][i];
            uint8_t n3 = keyMatrix[r][2] * textMatrix[2][i];
            
            resultMatrix[r][i] = n1 + n2 + n3;
            resultMatrix[r][i] %= 29;
        }
    }
    
    result = malloc(3 * textMatrixSize);
    
    for (size_t i = 0; i < (3 * textMatrixSize); i += 3)
        for (size_t j = 0; j < 3; j++)
            result[i + j] = unprocess(resultMatrix[j][i / 3]);
}

void runDecrypt()
{
    //
}

void freeEverything()
{
    free(resultMatrix[0]);
    free(resultMatrix[1]);
    free(resultMatrix[2]);
    
    free(textMatrix[0]);
    free(textMatrix[1]);
    free(textMatrix[2]);
}

int main(int argc, const char *const *argv)
{
    uint32_t v[21] = {563, 887, 433, 403, 389, 298, 533, 1113, 555, 916, 896, 646, 337, 809, 389, 568, 346, 403, 1143, 1653, 976};

    if (argc != 6)
    {
        fprintf(stderr, "Wrong Number of Arguments!\n");
        exit(1);
    }
    
    const char *message;
    const char *keyText;
    
    void (*runFunction)();
    
    if (argv[1][0] == '-' && argv[1][1] == 'e') {
        runFunction = runEncrypt;
    } else if (argv[1][0] == '-' && argv[1][1] == 'd') {
        runFunction = runDecrypt;
    } else {
        fprintf(stderr, "Invalid Argument!\n");
        exit(1);
    }
    
    if (argv[2][0] == '-' && argv[2][1] == 't') {
        message = argv[3];
    } else {
        fprintf(stderr, "Invalid Argument!\n");
    }
    
    if (argv[4][0] == '-' && argv[4][1] == 'k') {
        keyText = argv[5];
    } else {
        fprintf(stderr, "Invalid Argument!\n");
    }
    
    parseText(message);
    parseKey(keyText);
    
    dumpMessage();
    putchar('\n');
    dumpMessageRaw();
    putchar('\n');
    dumpKey();
    putchar('\n');
    
    runFunction();
    dumpResult();
    putchar('\n');
    dumpResultRaw();
    
    putchar('\n');
    printf("%s --> %s\n", text, result);

    freeEverything();
    return(0);
}
