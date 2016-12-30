/* 
See "version.h" for version info and copyright information
This file was last modified on 05.16.1999
*/

#include <cstdlib>
#include <windows.h>

#include "auxil.h"

// Pause for iTime milliseconds
void Pause(const unsigned long ulTime) {
	Sleep(ulTime);
}

// Return the number of milliseconds since Windows was started
unsigned long CurrentTime() {
	return 0;//timeGetTime();
}

// Return true if iInterval milliseconds have elapsed since
// the last time ElapsedTime was called.  Returns false on the 
// first call
bool ElapsedTime(const unsigned long ulInterval) {
	static unsigned long ulLastTime = 0;//timeGetTime();
	unsigned long ulCurrentTime;

	ulCurrentTime = 0;//timeGetTime();
	if((ulCurrentTime - ulLastTime) >= ulInterval) {
		ulLastTime = ulCurrentTime;
		return true;
	}
	return false;
}
