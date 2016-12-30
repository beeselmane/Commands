#include <car-priv.h>
#include <crv1.h>
#include <car.h>

//#include <CoreFoundation/CoreFoundation.h>

#define ARCHIVE "/Users/developer/Desktop/llvm.car"
#define FOLDER  "/Users/developer/Desktop/llvm"
//#define TAKE    "/WiiExtractor/WiiExtractor/DiscIO/Common/CDUtils.h"
//#define OP      "/Users/developer/Desktop/CDUtils.h"

int main(int argc, const char *argv[])
{
    //CFAbsoluteTime ctime = CFAbsoluteTimeGetCurrent();
    //CString *error = malloc(sizeof(CString));
    //CRv1CreateCAR(FOLDER, ARCHIVE, error, true, true);
    //UInt64 count = 0;
    //CString *files = CRv1ListFiles(ARCHIVE, &count, 0);
    //CRv1GrabFile(ARCHIVE, TAKE, OP, 0);
    //UInt64 count;
    //CString *files = CRv1ListFiles(ARCHIVE, &count, 0);
    CString error;
    CRv1CreateCAR(FOLDER, ARCHIVE, &error, true, false);
    //ctime = CFAbsoluteTimeGetCurrent() - ctime;

    //for (int i = 0; i < count; i++) printf("%s\n", files[i]);
    //printf("%s\n", files[200]);
    //printf("%f\n", ctime);

    return(0);
}
