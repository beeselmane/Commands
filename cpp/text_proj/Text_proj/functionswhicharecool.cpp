#include <iostream>
#include "S:\cpp\cpp\apstring.h"

using namespace std;

void getName(const char which, apstring &var);
apstring fml (apstring first,apstring middle,apstring last);
apstring lcfm (apstring first,apstring middle,apstring last);
apstring fmil (apstring first,apstring middle,apstring last);
apstring lcfmi (apstring first,apstring middle,apstring last);

int main(void)
{
	apstring first,middle,last;
	getName('f', first);
	getName('s', middle);
	getName('l', last);
	cout << fml(first,middle,last) << endl
		<< lcfm(first,middle,last) << endl
		<< fmil(first,middle,last) << endl
		<< lcfmi(first,middle,last) << endl;
	return(0);
}

void getName(const char which, apstring &var)
{
	if (which == 'f')
	{
		cout << "What is your first name? ";
	}
	else if (which == 's')
	{
		cout << "What is your middle name? ";
	}
	else if (which == 'l')
	{
		cout << "What is your last name? ";
	}
	else
	{
		cout << "???";
	}

	cin >> var;
}

apstring fml (apstring first,apstring middle,apstring last)
{
	return (first + " " + middle + " " + last);
}

apstring lcfm (apstring first,apstring middle,apstring last)
{
	return (last + ", " + first + " " + middle);
}

apstring fmil (apstring first,apstring middle,apstring last)
{
	return (first + " " + middle[0] + ". " + last);
}

apstring lcfmi (apstring first,apstring middle,apstring last)
{
	return (last + ", " + first + " " + middle[0] + ".");
}