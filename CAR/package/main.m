#import <Foundation/Foundation.h>
#import <CAR/CAR.h>

int main(int argc, const char *const *argv)
{
    @autoreleasepool
    {
        if (argc != 2)
        {
            fprintf(stderr, "Usage: %s <Root Directory>\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        NSString *packageDirectory = [[NSString stringWithUTF8String:argv[1]] stringByStandardizingPath];
        NSString *packageFilePath = [packageDirectory stringByAppendingString:@".car"];
        NSFileManager *fileManager = [NSFileManager defaultManager];
        BOOL directoryFlag = NO;

        if (![fileManager fileExistsAtPath:packageDirectory isDirectory:&directoryFlag])
        {
            fprintf(stderr, "Error: The file at the given path '%s' does not exist!\n", argv[1]);
            exit(EXIT_FAILURE);
        }

        if (!directoryFlag)
        {
            fprintf(stderr, "Error: The file at the given path '%s' is not a directory!\n", argv[1]);
            exit(EXIT_FAILURE);
        }

        if ([fileManager fileExistsAtPath:packageFilePath])
        {
            fprintf(stderr, "Warning: File already exists at output path '%s'! Override [y/n]? ", [packageFilePath UTF8String]);
            int c = fgetc(stdin);

            if (c != 'y') exit(EXIT_FAILURE);
        }

        fprintf(stdout, "Info: Creating Package of directory %s (%s) at path '%s'...", argv[1], [packageDirectory UTF8String], [packageFilePath UTF8String]);
        fflush(stdout);
        CXCARArchive *archive = [[CXCARArchive alloc] initWithRootDirectoryAtPath:packageDirectory];
        fprintf(stdout, "\nInfo: Loaded %lu resources for package", [archive numberOfItems]);
        fprintf(stdout, "\nInfo: Writing to package file...");
        fflush(stdout);
        BOOL success = [archive writeArchiveToPath:packageFilePath];

        if (success) {
            fprintf(stdout, "\nInfo: Done\n");
            exit(EXIT_SUCCESS);
        } else {
            fprintf(stderr, "\nError: Could not write package file to path '%s'\n", [packageFilePath UTF8String]);
            exit(EXIT_FAILURE);
        }
    }
}
