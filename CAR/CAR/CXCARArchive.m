#import <CAR/CAR.h>
#import "OSCAR.h"

extern UInt32 CXCRC32Initialize(void);
extern UInt32 CXCRC32Update(UInt32 current, void *buffer, UInt64 size);
extern UInt32 CXCRC32Finish(UInt32 current);
extern UInt32 CXCRC32Calculate(void *buffer, UInt64 size);

const UInt8 kCXCARItemTypeUnknown = -1;
const UInt8 kCXCARItemTypeDirectory = kCARTOCEntryTypeDirectory;
const UInt8 kCXCARItemTypeFile = kCARTOCEntryTypeFile;
const UInt8 kCXCARItemTypeSymbolicLink = kCARTOCEntryTypeSymbolicLink;

@interface CXCARArchiveData : NSObject
{
    @public
        NSString *itemPath;
        CARTOCEntryType type;

    NSData *asFile;
    NSArray *asDirectory;
    NSString *asSymlink;

    id parent;
}

@end

@implementation CXCARArchiveData

@end

NSArray *CXCARDirectoryContentsRecurse(NSString *path, NSString *root)
{
    NSFileManager *fm = [NSFileManager defaultManager];
    NSMutableArray *result = [NSMutableArray array];

    CXCARArchiveData *thisData = [[CXCARArchiveData alloc] init];
    thisData->itemPath = [path substringFromIndex:[root length]];
    thisData->asDirectory = [NSMutableArray array];
    thisData->type = kCARTOCEntryTypeDirectory;
    [result addObject:thisData];

    NSArray *contents = [fm contentsOfDirectoryAtPath:path error:nil];
    if (!contents) return nil;

    for (NSString *entry in contents)
    {
        if ([entry isEqualToString:@"."] || [entry isEqualToString:@".."]) continue;
        if ([entry isEqualToString:@".DS_Store"]) continue;

        NSMutableArray *thisDirContents = (NSMutableArray *)thisData->asDirectory;
        [thisDirContents addObject:entry];

        NSString *fullpath = [NSString stringWithFormat:@"%@/%@", path, entry];
        NSDictionary *attributes = [fm attributesOfItemAtPath:fullpath error:nil];

        if ([[attributes objectForKey:NSFileType] isEqualToString:NSFileTypeSymbolicLink]) {
            CXCARArchiveData *entryData = [[CXCARArchiveData alloc] init];
            NSString *destination = [fm destinationOfSymbolicLinkAtPath:fullpath error:nil];
            entryData->itemPath = [NSString stringWithFormat:@"%@/%@", thisData->itemPath, entry];
            entryData->type = kCARTOCEntryTypeSymbolicLink;
            entryData->asSymlink = destination;

            [result addObject:entryData];
        } else if ([[attributes objectForKey:NSFileType] isEqualToString:NSFileTypeRegular]) {
            CXCARArchiveData *entryData = [[CXCARArchiveData alloc] init];
            NSData *contents = [NSData dataWithContentsOfFile:fullpath];
            entryData->itemPath = [NSString stringWithFormat:@"%@/%@", thisData->itemPath, entry];
            entryData->type = kCARTOCEntryTypeFile;
            entryData->asFile = contents;

            [result addObject:entryData];
        } else if ([[attributes objectForKey:NSFileType] isEqualToString:NSFileTypeDirectory]) {
            NSString *subDirPath = [NSString stringWithFormat:@"%@/%@", path, entry];
            NSArray *subdirectory = CXCARDirectoryContentsRecurse(subDirPath, root);
            [result addObjectsFromArray:subdirectory];
        } else {
            NSLog(@"Invalid File Type '%@'!", [attributes objectForKey:NSFileType]);
        }
    }

    if (![thisData->itemPath length]) thisData->itemPath = @"/";
    return result;
}

@interface CXCARArchive (Private)

- (CXCARArchiveData *) archiveEntryDataForPath:(NSString *)path;
- (NSInteger) findItemAtPath:(NSString *)path;
- (void) recalculateHeader;

- (NSString *) colonInPathString;
- (NSString *) rootDirectoryString;
- (NSString *) pathSeparatorString;
- (NSArray *) fullContents;

@end

@implementation CXCARArchive
{
    CARHeaderS1 archiveHeader;
    NSCache *lookupCache;

    NSMutableArray <CXCARArchiveData *>*data;
    UInt64 entryCount;
}

+ (instancetype) archiveAtPath:(NSString *)path
{
    return [[[self class] alloc] initWithArchiveAtPath:path];
}

- (instancetype) initWithRootDirectoryAtPath:(NSString *)path
{
    if (self = [self init])
    {
        path = [path stringByStandardizingPath];
        data = (NSMutableArray *)CXCARDirectoryContentsRecurse(path, path);
        entryCount = [data count];
        [self recalculateHeader];
    }

    return self;
}

- (instancetype) initWithArchiveAtPath:(NSString *)path
{
    if (self = [self init])
    {
        FILE *fp = fopen([path UTF8String], "rb");
        if (!fp) return nil;

        fread(&archiveHeader, sizeof(CARHeaderS1), 1, fp);
        entryCount = archiveHeader.stringTableOffset - archiveHeader.tocOffset;
        entryCount /= sizeof(CARTOCEntry);
        CXCARArchiveData *currentDirectoryHandle = nil;
        UInt64 offset = archiveHeader.tocOffset;
        data = [NSMutableArray array];
        // UInt32 checksum = CXCRC32Initialize();
        // TODO: Write checksum

        for (UInt64 i = 0; i < entryCount; i++)
        {
            fseek(fp, offset, SEEK_SET);
            CARTOCEntry entry;
            fread(&entry, sizeof(CARTOCEntry), 1, fp);
            CXCARArchiveData *archiveEntryData = [CXCARArchiveData new];
            archiveEntryData->type = entry.entryType;

            offset = ftell(fp);
            fseek(fp, archiveHeader.stringTableOffset + entry.nameOffset, SEEK_SET);
            UInt8 nameBuffer[1024]; char n = -1; int j = 0;
            while ((n = fgetc(fp))) nameBuffer[j++] = n;
            nameBuffer[j] = 0;

            NSString *basePathString = [NSString stringWithUTF8String:(const char *)nameBuffer];
            NSString *slashString = [basePathString stringByReplacingOccurrencesOfString:[self pathSeparatorString] withString:@"/"];
            NSString *rootString = [slashString stringByReplacingCharactersInRange:NSMakeRange(0, 1) withString:@"/"];
            archiveEntryData->itemPath = [rootString stringByReplacingOccurrencesOfString:[self colonInPathString] withString:@":"];

            if (currentDirectoryHandle)
            {
                if (![archiveEntryData->itemPath hasPrefix:currentDirectoryHandle->itemPath])
                    currentDirectoryHandle = currentDirectoryHandle->parent;

                if ([[archiveEntryData->itemPath stringByDeletingLastPathComponent] isEqualToString:currentDirectoryHandle->itemPath])
                {
                    NSMutableArray *parentDirectoryContents = (NSMutableArray *)currentDirectoryHandle->asDirectory;
                    [parentDirectoryContents addObject:archiveEntryData->itemPath];
                }
            }

            if (entry.dataSize) {
                fseek(fp, archiveHeader.dataSectionOffset + entry.dataOffset, SEEK_SET);
                UInt8 *buffer = malloc(entry.dataSize);
                fread(buffer, entry.dataSize, 1, fp);

                switch (archiveEntryData->type)
                {
                    case kCARTOCEntryTypeFile: {
                        archiveEntryData->asFile = [NSData dataWithBytes:buffer length:entry.dataSize];
                        free(buffer);
                    } break;
                    case kCARTOCEntryTypeSymbolicLink: {
                        archiveEntryData->asSymlink = [NSString stringWithUTF8String:(const char *)buffer];
                        free(buffer);
                    } break;
                    default: break;
                }
            } else {
                archiveEntryData->parent = currentDirectoryHandle;
                currentDirectoryHandle = archiveEntryData;

                archiveEntryData->asDirectory = [NSMutableArray array];
            }

            data[i] = archiveEntryData;
        }
    }

    return self;
}

- (instancetype) init
{
    if (self = [super init])
    {
        lookupCache = [[NSCache alloc] init];
        [lookupCache setCountLimit:512];
    }

    return self;
}

- (NSInteger) findItemAtPath:(NSString *)path
{
    for (NSInteger i = 0; i < [data count]; i++)
        if ([data[i]->itemPath isEqualToString:path])
            return i;

    return -1;
}

- (CXCARArchiveData *) archiveEntryDataForPath:(NSString *)path
{
    NSNumber *itemNumber = [lookupCache objectForKey:path];
    NSInteger itemIndex = -1;

    if (itemNumber) {
        itemIndex = [itemNumber integerValue];
    } else {
        itemIndex = [self findItemAtPath:path];

        if (itemIndex != -1) {
            [lookupCache setObject:@(itemIndex) forKey:path];
        } else {
            return nil;
        }
    }

    return data[itemIndex];
}

- (NSString *) colonInPathString
{
    static NSString *pathColonString = nil;
    if (!pathColonString) pathColonString = [NSString stringWithCString:"\xEE\xEE" encoding:NSUTF16BigEndianStringEncoding];
    return pathColonString;
}

- (NSString *) rootDirectoryString
{
    static NSString *rootDirectoryString = nil;
    if (!rootDirectoryString) rootDirectoryString = [NSString stringWithFormat:@"%c", kCARRootPathCharacter];
    return rootDirectoryString;
}

- (NSString *) pathSeparatorString
{
    static NSString *pathSeparatorString = nil;
    if (!pathSeparatorString) pathSeparatorString = [NSString stringWithFormat:@"%c", kCARPathSeparatorCharacter];
    return pathSeparatorString;
}

- (void) recalculateHeader
{
    CARHeaderS1 header = {
        .magic = 0,
        .version = 0,
        .tocOffset = sizeof(CARHeaderS1),
        .stringTableOffset = (sizeof(CARTOCEntry) * entryCount) + sizeof(CARHeaderS1),
        .dataSectionOffset = 0,
        .checksum = 0
    };

    memcpy(&header.version, kCARHeaderVersionS1, 4);
    memcpy(&header.magic, kCARHeaderMagic, 4);

    archiveHeader = header;
}

- (BOOL) writeItemAtPath:(NSString *)itemPath toPath:(NSString *)path
{
    CXCARArchiveData *entry = [self archiveEntryDataForPath:itemPath];
    NSData *itemData = (entry ? entry->asFile : nil);
    if (!itemData) return NO;

    return [itemData writeToFile:path atomically:YES];
}

- (BOOL) writeArchiveToPath:(NSString *)path
{
    FILE *fp = fopen([path UTF8String], "wb");
    if (!fp) return NO;

    [self recalculateHeader];
    archiveHeader.checksum = CXCRC32Initialize();
    fseek(fp, sizeof(CARHeaderS1), SEEK_SET);
    UInt32 stringOffset = 0;
    UInt64 dataOffset = 0;

    for (UInt64 i = 0; i < entryCount; i++)
    {
        CARTOCEntry tocEntry = {
            .nameOffset = stringOffset,
            .entryType = data[i]->type,
            .dataOffset = 0,
            .dataSize = 0
        };

        switch (tocEntry.entryType)
        {
            case kCARTOCEntryTypeFile: {
                tocEntry.dataOffset = dataOffset;
                tocEntry.dataSize = [data[i]->asFile length];
            } break;
            case kCARTOCEntryTypeSymbolicLink: {
                tocEntry.dataOffset = dataOffset;
                tocEntry.dataSize = [[data[i]->asSymlink dataUsingEncoding:NSUTF8StringEncoding] length] + 1;
            } break;
            default: break;
        }

        fwrite(&tocEntry, sizeof(CARTOCEntry), 1, fp);
        archiveHeader.checksum = CXCRC32Update(archiveHeader.checksum, &tocEntry, sizeof(CARTOCEntry));
        stringOffset += [[data[i]->itemPath dataUsingEncoding:NSUTF8StringEncoding] length] + 1;
        dataOffset += tocEntry.dataSize;
    }

    for (UInt64 i = 0; i < entryCount; i++)
    {
        NSString *entryRootCorrected = [data[i]->itemPath stringByReplacingCharactersInRange:NSMakeRange(0, 1) withString:[self rootDirectoryString]];
        NSString *entryPathString = [entryRootCorrected stringByReplacingOccurrencesOfString:@":" withString:[self colonInPathString]];
        NSString *entryPathBackPath = [entryPathString stringByReplacingOccurrencesOfString:@"/" withString:[self pathSeparatorString]];
        NSString *entryPathFinal = [entryPathBackPath stringByReplacingOccurrencesOfString:@"\\" withString:[self pathSeparatorString]];

        NSMutableData *stringData = [[entryPathFinal dataUsingEncoding:NSUTF8StringEncoding] mutableCopy];
        [stringData appendBytes:"\0" length:1];
        archiveHeader.checksum = CXCRC32Update(archiveHeader.checksum, (void *)[stringData bytes], [stringData length]);
        fwrite([stringData bytes], [stringData length], 1, fp);
    }

    archiveHeader.dataSectionOffset = ftell(fp);

    for (UInt64 i = 0; i < entryCount; i++)
    {
        NSData *entryData = nil;

        switch (data[i]->type)
        {
            case kCARTOCEntryTypeFile: {
                entryData = data[i]->asFile;
            } break;
            case kCARTOCEntryTypeSymbolicLink: {
                NSMutableData *entryDataMutable = [[data[i]->asSymlink dataUsingEncoding:NSUTF8StringEncoding] mutableCopy];
                [entryDataMutable appendBytes:"\0" length:1];
                entryData = entryDataMutable;
            } break;
            default: break;
        }

        if (entryData)
        {
            archiveHeader.checksum = CXCRC32Update(archiveHeader.checksum, (void *)[entryData bytes], [entryData length]);
            fwrite([entryData bytes], [entryData length], 1, fp);
        }
    }

    archiveHeader.checksum = CXCRC32Update(archiveHeader.checksum, &archiveHeader, sizeof(CARHeaderS1) - sizeof(UInt32));

    fseek(fp, 0, SEEK_SET);
    fwrite(&archiveHeader, sizeof(CARHeaderS1), 1, fp);
    fclose(fp);

    return YES;
}

- (NSArray *) contentsOfDirectoryAtPath:(NSString *)path followingSymbolicLinks:(BOOL)follow
{
    CXCARArchiveData *entry = [self archiveEntryDataForPath:path];
    if (!entry) return nil;

    if (follow && entry->type == kCARTOCEntryTypeSymbolicLink)
        return [self contentsOfDirectoryAtPath:path followingSymbolicLinks:follow];

    return entry->asDirectory;
}

- (NSArray *) contentsOfDirectoryAtPath:(NSString *)path
{
    return [self contentsOfDirectoryAtPath:path followingSymbolicLinks:YES];
}

- (NSData *) contentsOfItemAtPath:(NSString *)path followingSymbolicLinks:(BOOL)follow
{
    CXCARArchiveData *entry = [self archiveEntryDataForPath:path];
    if (!entry) return nil;
    NSData *result = nil;

    if (follow && entry->type == kCARTOCEntryTypeSymbolicLink)
        return [self contentsOfItemAtPath:entry->asSymlink followingSymbolicLinks:YES];

    if (entry->type == kCARTOCEntryTypeFile) {
        result = entry->asFile;
    } else if (entry->type == kCARTOCEntryTypeSymbolicLink) {
        result = (id)entry->asSymlink;
    }

    return result;
}

- (NSData *) contentsOfItemAtPath:(NSString *)path
{
    return [self contentsOfItemAtPath:path followingSymbolicLinks:YES];
}

- (NSString *) locationOfSymbolicLinkAtPath:(NSString *)path
{
    CXCARArchiveData *entry = [self archiveEntryDataForPath:path];
    return (entry ? entry->asSymlink : nil);
}

- (UInt8) typeOfItemAtPath:(NSString *)path
{
    CXCARArchiveData *entry = [self archiveEntryDataForPath:path];
    return (entry ? entry->type : kCXCARItemTypeUnknown);
}

- (NSUInteger) numberOfItems
{
    return [data count];
}

- (NSArray *) fullContents
{
    NSMutableArray *result = [NSMutableArray array];
    for (CXCARArchiveData *entry in data) [result addObject:entry->itemPath];
    return result;
}

- (NSData *) dataForPath:(NSString *)path
{
    return [self contentsOfItemAtPath:path];
}

@end
