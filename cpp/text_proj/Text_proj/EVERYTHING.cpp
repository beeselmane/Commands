// standard header include
#include <iostream>

// preproccessor chunk (unused because of second define)
#define toocool4u 1
#define __besselman_cpp__ 1

#ifndef __besselman_cpp__
#define __besselman_cpp__ toocool4u

#define null 1
#define bool int
#define true 0
#define false 1

#endif /** __besselman_cpp__ */

// namespace declaration
using std::cin;
using std::cout;
using std::endl;

// function definitions
void start();
void list_func();
void select();
bool func_sel(int which_function_to_select);
void list_func();
void func_addr(char perform_normally);
void func_arith();
void func_age();
void func_name();

// global variables
char selector;
char term;

/**
* exit: exit program
* prgm: print this list
* a: address list program
* b: arithmatic program
* c: ASCII art
* d: Age program
* f: Name program
*/

// main program function
int main(void)
{
	start();
	cin.ignore();
	return 0;
}

// starts up the program
void start(void)
{
	cout << "Please Enter A Program To Run(p to list): ";
	cin >> selector;
	select();
}

// runs the selection function and dictates all program flow
void select(void)
{
	if ((selector == 'e') || (selector == '\0'))
	{
		cout << "Terminate Program?(Y/N): ";
		cin >> term;

		if ((term == 'Y') || (term == 'y'))
		{
			return;
		}
		else if ((term == 'N') || (term == 'n'))
		{
			start();
		}
		else
		{
			cout << "Unrecognised Response...\r\n Terminating...\r\n";
			return;
		}
	}
	else if (selector == 'p')
	{
		list_func();
		start();
	}
	else if (selector == 'a')
	{
		func_sel(1);
		start();
	}
	else if (selector == 'b')
	{
		func_sel(2);
		start();
	}
	else if (selector == 'c')
	{
		func_sel(3);
		start();
	}
	else if (selector == 'd')
	{
		func_sel(4);
		start();
	}
	else if (selector == 'f')
	{
		func_sel(5);
		start();
	}
	else
	{
		cout << "Unrecognized response...\r\n";
		cout << "Please Enter A Valid Program: ";
		cin >> selector;
		select();
	}
}

// selects all functions via an integer
bool func_sel(int func)
{
	switch (func)
	{
		case 1:
			func_addr('n');
		break;
		case 2:
			func_arith();
		break;
		case 3:
		break;
		case 4:
			func_age();
		break;
		case 5:
			func_name();
		break;
		default:
			return false;
		break;
	}

	return true;
}

// Lists all functions
void list_func()
{
	cout << "List of all existing functions:\r\n"
	<< "p: print this list\r\n"
	<< "e: exit program\r\n"
	<< "a: address list program\r\n"
	<< "b: arithmatic program\r\n"
	<< "c: ASCII art\r\n"
	<< "d: Age program\r\n"
	<< "f: Name program\r\n";
	cin.ignore();
}

// Adress Function
void func_addr(char normalize)
{
	if (normalize == 'n')
	{
		cout << "*****************************************\r\n";
		cout << "*    Tyler Besselman, Pittsburgh, PA    *\r\n";
		cout << "* 2059 Blairmont Drive, Upper St. Clair *\r\n";
		cout << "*****************************************\r\n";
		cin.ignore();
	}
	else
	{
		for ( ; ; )
		{
			cout << "ERROR\r\n";
		}
	}
}

void func_arith()
{
	char choice;
	cout << "Would you like to do (b)basic arithmatic or (c)complex arithmatic?\r\n";
	cin >> choice;

	if (choice == 'b')
	{
		cout << "1 + 7 = " << 1 + 7 << endl
		<< "7 - 2 = " << 7 - 2 << endl
		<< "2 * 3 = " << 2 * 3  << endl
		<< "5 / 2 = " << 5 / 2.00 << endl
		<< "52 % 9 = " << 52 % 9 << endl;
		cin.ignore();
	}
	else if (choice == 'c')
	{
		char op;
		double a, b;

		cout << "What operation would you like to choose... (a)Addition, (s)Subtraction, (m)Multiplication, (d)Division, or (M)Modulous?\r\n";
		cin >> op;

		cout << "Number 1: " << endl;
		cin >> a;

		cout << "Number 2: " << endl;
		cin >> b;

		if (op == 'a')
		{
			cout << a << " + " << b << " = " << a + b << endl;
		}
		else if (op == 's')
		{
			cout << a << " - " << b << " = " << a - b << endl;
		}
		else if (op == 'm')
		{
			cout << a << " * " << b << " = " << a * b << endl;
		}
		else if (op == 'd')
		{
			cout << a << " / " << b << " = " << a / b << endl;
		}
		else if (op == 'M')
		{
			cout << (int) a << " % " << (int) b << " = " << (int)a % (int)b << endl;
		}
		else
		{
			cout << "Unrecognized Choice...\r\n";
			cin.ignore();
			start();
		}

		cin.ignore();
	}
	else
	{
		cout << "Unrecognized choice...\r\n defaulting to basic...\r\n";
	}
}

void func_age()
{
	int age, year;

	cout << "Please Enter you Birth Year: ";
	cin >> age;

	cout << "Please Enter the Current Year: ";
	cin >> year;

	if (age >= year)
	{
		cout << "You cant be negative years old!\r\n";
			cin.ignore();
	}
	else
	{
		cout << "You are " << year - age << " Years Old.\r\n";
		cin.ignore();
	}
}

void func_name()
{
	char first, middle, last;

	cout << "Please Input your first initial: ";
	cin >> first;

	cout << "Please Input your second initial: ";
	cin >> middle;

	cout << "Please Input your last initial: ";
	cin >> last;

	cout << "Welcome, " << first << middle << last << "!" << endl;
}