#include <iostream>

using namespace std;

// Functions
void func_addr();    // address function
void func_arith();   // arithmatic function
void func_age();     // age function
void func_name();    // name function
void func_temp();    // temperature function

// Main Function
int main(void)
{
	func_addr();
	cin.ignore();
	cout << endl;
	func_arith();
	cin.ignore();
	cout << endl;
	func_age();
	cin.ignore();
	cout << endl;
	func_name();
	cin.ignore();
	cout << endl;
	func_temp();
	return 0;
}

// Address Function
void func_addr()
{
	cout << "*****************************************\r\n";
	cout << "*    Tyler Besselman, Pittsburgh, PA    *\r\n";
	cout << "* 2059 Blairmont Drive, Upper St. Clair *\r\n";
	cout << "*****************************************\r\n";
}

// Math Function
void func_arith()
{
	char choice;
	cout << "Would you like to do (b)basic arithmatic or (c)complex arithmatic?" << endl;
	cin >> choice;

	if (choice == 'b')
	{
		cout << "1 + 7 = " << 1 + 7 << endl
		<< "7 - 2 = " << 7 - 2 << endl
		<< "2 * 3 = " << 2 * 3  << endl
		<< "5 / 2 = " << 5 / 2.00 << endl
		<< "52 % 9 = " << 52 % 9 << endl;
	}
	else if (choice == 'c')
	{
		char op;
		double a, b;

		cout << "What operation would you like to choose... (a)Addition, (s)Subtraction, (m)Multiplication, (d)Division, or (M)Modulous?" << endl;
		cin >> op;

		cout << "Number 1: ";
		cin >> a;

		cout << "Number 2: ";
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
			cout << "Unrecognized Choice..." << endl;
		}
	}
	else
	{
		cout << "Unrecognized choice..." << endl;
	}
}

// Age Function
void func_age()
{
	int age, year;

	cout << "Please Enter you Birth Year: ";
	cin >> age;

	cout << "Please Enter the Current Year: ";
	cin >> year;

	if (age >= year)
	{
		cout << "You cant be negative years old!" << endl;
	}
	else
	{
		cout << "You are " << year - age << " Years Old." << endl;
	}
}

// Name Function
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

// Temperature Function
void func_temp()
{
	double temp_f, temp_c, temp_k;

	cout << "Please Input the Temperature In Fahrenheit: ";
	cin >> temp_f;

	temp_c = (temp_f - 32) * (5.000 / 9.000);
	temp_k = temp_c + 273;

	cout << endl << "Temperatures:" << endl;
	cout << temp_f << "øF" << endl << temp_c << "øC" << endl << temp_k << "øK" << endl;
}