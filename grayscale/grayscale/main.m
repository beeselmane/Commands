#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <Cocoa/Cocoa.h>

@interface NSImage (Grayscale)

- (NSImage *) grayscaleImage;
- (NSImage *) invertedImage;

@end

@implementation NSImage (Grayscale)

- (NSImage *)grayscaleImage
{
    NSImage *outputImage = [[NSImage alloc] initWithSize:[self size]];
    [outputImage lockFocus];

    CIFilter *monochromeFilter = [CIFilter filterWithName:@"CIColorMonochrome"]; [monochromeFilter setDefaults];
    [monochromeFilter setValue:[CIImage imageWithData:[self TIFFRepresentation]] forKey:@"inputImage"];
    [monochromeFilter setValue:[CIColor colorWithCGColor:[[NSColor lightGrayColor] CGColor]] forKey:@"inputColor"];
    [monochromeFilter setValue:[NSNumber numberWithFloat:1.0F] forKey:@"inputIntensity"];

    CIImage *output = [monochromeFilter valueForKey:@"outputImage"];
    [output drawAtPoint:NSZeroPoint fromRect:(NSRect){NSZeroPoint, [self size]} operation:NSCompositeCopy fraction:1.0F];
    [outputImage unlockFocus];

    return outputImage;
}

- (NSImage *) invertedImage
{
    NSImage *outputImage = [[NSImage alloc] initWithSize:[self size]];
    [outputImage lockFocus];

    CIFilter *filter = [CIFilter filterWithName:@"CIColorInvert"]; [filter setDefaults];
    [filter setValue:[CIImage imageWithData:[self TIFFRepresentation]] forKey:@"inputImage"];

    CIImage *output = [filter valueForKey:@"outputImage"];
    [output drawAtPoint:NSZeroPoint fromRect:(NSRect){NSZeroPoint, [self size]} operation:NSCompositeCopy fraction:1.0F];
    [outputImage unlockFocus];

    return outputImage;
}

@end

static void usage(const char *name)
{
    fprintf(stderr, "Usage: %s -gi input output\n", name);
    fprintf(stderr, "       -g: Grayscale Image\n");
    fprintf(stderr, "       -i: Invert Image   \n");

    exit(1);
}

static void single(SEL operator, NSString *inputPath, NSString *outputPath)
{
    NSImage *inputImage = [[NSImage alloc] initWithContentsOfFile:inputPath];

    if (!inputImage)
    {
        fprintf(stderr, "Error: Could not open image at path '%s'\n", [inputPath UTF8String]);
        exit(1);
    }

    NSImage *outputImage = [inputImage performSelector:operator]; [outputImage lockFocus];
    NSBitmapImageRep *bmp = [[NSBitmapImageRep alloc] initWithFocusedViewRect:(NSRect){NSZeroPoint, [outputImage size]}];
    [outputImage unlockFocus];

    NSData *encodedImage = [bmp representationUsingType:NSPNGFileType properties:@{}];
    [encodedImage writeToFile:outputPath atomically:YES];
}

int main(int argc, const char **argv)
{
    @autoreleasepool
    {
        if (argc < 3)
        {
            fprintf(stderr, "%d", argc);
            usage(argv[0]);
        }

        SEL imageOperator = nil;
        BOOL replace = NO;

        if (argv[1][0] != '-') {
            usage(argv[0]);
        } else if (argv[1][1] == 'g') {
            imageOperator = @selector(grayscaleImage);
        } else if (argv[1][1] == 'i') {
            imageOperator = @selector(invertedImage);
        } else {
            usage(argv[0]);
        }

        if (argv[1][2] == 'r') {
            replace = YES;
        } else if (argv[1][2] != '\0') {
            usage(argv[0]);
        }

        if (replace) {
            for (int i = 2; i < argc; i++)
            {
                NSString *imagePath = [NSString stringWithUTF8String:argv[i]];
                single(imageOperator, imagePath, imagePath);
            }
        } else {
            single(imageOperator, [NSString stringWithUTF8String:argv[2]], [NSString stringWithUTF8String:argv[1]]);
        }
    }

    return(0);
}
