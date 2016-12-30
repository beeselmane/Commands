#import <CAR/CAR.h>

@interface CXCARArchive (Private)

- (NSArray *) fullContents;

@end

int main(int argc, const char *const *argv)
{
    @autoreleasepool
    {
        //CXCARArchive *archive = [[CXCARArchive alloc] initWithRootDirectoryAtPath:@"/home/developer/tmp/rd"];
        CXCARArchive *archive = [CXCARArchive archiveAtPath:@"/home/developer/tmp/rd.car"];
        NSLog(@"%@", [archive fullContents]);
        NSLog(@"%@", [[NSString alloc] initWithData:[archive dataForPath:@"/a.txt"] encoding:NSUTF8StringEncoding]);
    }

    return 0;
}
