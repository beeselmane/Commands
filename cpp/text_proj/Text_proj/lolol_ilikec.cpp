/** #include <iostream>
using std::cin;

extern "C"
{
	#include <stdio.h>

	void this_is_not_my_main_function(int hiiiii)
	{
		printf("hello\r\n");
	}
}

int main() { cin.ignore(); return 0; } */