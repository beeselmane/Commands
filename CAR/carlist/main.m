#import <Foundation/Foundation.h>
#import <CAR/CAR.h>

@interface CXCARArchive (Private)

- (NSArray *) fullContents;

@end

int main(int argc, const char *const *argv)
{
    @autoreleasepool
    {
        if (argc != 2)
        {
            fprintf(stderr, "Usage: %s <CAR Archive>", argv[0]);
            exit(EXIT_FAILURE);
        }

        NSString *archivePath = [[NSString stringWithUTF8String:argv[1]] stringByStandardizingPath];
        fprintf(stdout, "Contents of Archive at path '%s' are as follows:\n", [archivePath UTF8String]);
        fprintf(stdout, "%s", [[[[CXCARArchive archiveAtPath:archivePath] fullContents] description] UTF8String]);
        exit(EXIT_SUCCESS);
    }
}
