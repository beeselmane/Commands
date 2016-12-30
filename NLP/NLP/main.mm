#import <Foundation/NSString.h>
#import <Foundation/NSArray.h>
#import <stdlib.h>
#import <stdio.h>

NSArray *next()
{
    char *line = NULL;
    size_t size = 0;

    printf(":~> ");
    ssize_t read = getline(&line, &size, stdin);

    // Some weird stdin error? (I just need to use read -.-)
    if (read == -1) return nil;

    NSMutableArray *array = [[NSMutableArray alloc] init];
    NSString *currentArrayWord = nil;
    char currentWord[256];
    BOOL wasSpace = NO;
    int cwIndex = 0;

    for (size_t i = 0; i < size; i++)
    {
        if (line[i] == ' ' || line[i] == '\n')
        {
            if (wasSpace) continue;

            currentWord[cwIndex++] = 0;
            currentArrayWord = [NSString stringWithUTF8String:currentWord];
            [array addObject:currentArrayWord];
            wasSpace = YES;
            cwIndex = 0;

            continue;
        }

        currentWord[cwIndex++] = line[i];
        wasSpace = NO;
    }

    free(line);
    return(array);
}

int main(int argc, const char *argv[])
{
    @autoreleasepool
    {
        NSArray *currentLine = nil;

        while ((currentLine = next()))
        {
            if ([currentLine[0] isEqualToString:@"EXIT"]) break;

            if ([currentLine[0] length] == 0)
            {
                printf("eh?\n");
                continue;
            }

            NSLog(@"%@", currentLine);
        }
    }

    return(0);
}
