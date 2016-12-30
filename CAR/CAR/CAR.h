#ifndef __CAR__
#define __CAR__ 1

#import <Foundation/Foundation.h>

extern const UInt8 kCXCARItemTypeUnknown;
extern const UInt8 kCXCARItemTypeDirectory;
extern const UInt8 kCXCARItemTypeFile;
extern const UInt8 kCXCARItemTypeSymbolicLink;

@interface CXCARArchive : NSObject

+ (instancetype) archiveAtPath:(NSString *)path;
- (instancetype) initWithRootDirectoryAtPath:(NSString *)path;
- (instancetype) initWithArchiveAtPath:(NSString *)path;

- (BOOL) writeItemAtPath:(NSString *)itemPath toPath:(NSString *)path;
- (BOOL) writeArchiveToPath:(NSString *)path;

- (NSArray *) contentsOfDirectoryAtPath:(NSString *)path followingSymbolicLinks:(BOOL)follow;
- (NSArray *) contentsOfDirectoryAtPath:(NSString *)path;
- (NSData *) contentsOfItemAtPath:(NSString *)path followingSymbolicLinks:(BOOL)follow;
- (NSData *) contentsOfItemAtPath:(NSString *)path;
- (NSData *) dataForPath:(NSString *)path;
- (NSString *) locationOfSymbolicLinkAtPath:(NSString *)path;
- (UInt8) typeOfItemAtPath:(NSString *)path;
- (NSUInteger) numberOfItems;

@end

FOUNDATION_EXPORT const unsigned char CARVersionString[];
FOUNDATION_EXPORT double CARVersionNumber;

#endif /* !defined(__CAR__) */
