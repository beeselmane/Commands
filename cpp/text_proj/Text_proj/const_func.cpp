#include <iostream>
#include "S:cpp\apstring.h"

//NameSpace
using namespace std;

// Functions
void getName(apstring &name);
void getVacation(apstring &vacation_spot);
void nameOut(const apstring &name);
void vacOut(const apstring &vac, const apstring &name);
void str_asm(apstring &new_str, const apstring &vac, const apstring &name);

// Main
int main(void)
{
	apstring name, vacation, both;
	getName(name);
	getVacation(vacation);
	nameOut(name);
	vacOut(vacation, name);
	str_asm(both, vacation, name);
	return 0;
}

void getName(apstring &name)
{
	cout << "What is your name? ";
	getline(cin, name);
}

void getVacation(apstring &vac)
{
	cout << "What is your favorite vacation spot? ";
	getline(cin, vac);
}

void nameOut(const apstring &name)
{
	cout << name << " is cool!" << endl;
}

void vacOut(const apstring &vac, const apstring &name)
{
	cout << name << " loves to go to " << vac << endl;
}

void str_asm(apstring &new_str, const apstring &name, const apstring &vac)
{
	new_str = name + " loves to go to " + vac;
}