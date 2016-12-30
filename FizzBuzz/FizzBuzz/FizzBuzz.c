#include <CoreFoundation/CoreFoundation.h>

int array(void);
int stand(void);

void _asm(void);

int main(void)
{
#if 0
    freopen("/dev/null", "w", stdout);

    CFAbsoluteTime begintime = CFAbsoluteTimeGetCurrent();
    array();
    CFAbsoluteTime atime = CFAbsoluteTimeGetCurrent() - begintime;
    begintime = CFAbsoluteTimeGetCurrent();
    stand();
    CFAbsoluteTime stime = CFAbsoluteTimeGetCurrent() - begintime;

    freopen("/dev/stderr", "w", stdout);
    printf("array() took %f seconds\n", atime);
    printf("stand() took %f seconds\n", stime);
#else
    _asm();
#endif

    return(0);
}
