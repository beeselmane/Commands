/**#include <iostream>
using namespace std;
char *command = new(sizeof(char));

int main(void)
{
	cout << "Command Line... Enter Commands At The Prompt..." << endl;
	for ( ; ; )
	{
		cout << "C:/> ";
		cin >> command;
		system(command);
	}
	return 0;
} */