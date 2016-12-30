#include <iostream>
#include "S:\cpp\apstring.h"

double getHeight(apstring measurement);
double convert(double feet, double inches);
void printHeight(double thingy);

int main(void)
{
	printHeight(convert(getHeight("feet"), getHeight("inches")));
	return 0;
}

double getHeight(apstring type)
{
	if (type == "feet")
	{
		int feet;
		cout << "Please input your height in feet(no inches): ";
		cin >> feet;
		return feet;
	}
	if (type == "inches")
	{
		int inches;
		cout << "Please input how many inches you are: ";
		cin >> inches;
		return inches;
	}
	cout << "???" << endl;
	return 1;
}

double convert(double feet, double inches)
{
	double total = 12 * feet + inches;
	return (total * 2.54);
}

void printHeight(double num)
{
	cout << "Your height in centimeters: " << num << endl;
	return;
}