#include <iostream>
#include "S:\cpp\apstring.h"
#include "S:\cpp\randgen.h"

using namespace std;

apstring left(const apstring &text, int numchars);
apstring right(const apstring &text, int numchars);

bool game_start();

int main(void)
{
	bool winner;

	for ( ; ; )
	{
		winner = game_start();
		if (winner)
		{
			cout << "You win!!" << endl;
			break;
		}
	}
		

	return(0);
}

apstring left(const apstring &text, int numchars)
{
	return (text.substr(0, numchars));
}

apstring right(const apstring &text, int numchars)
{
	return (text.substr((text.length() - numchars), numchars));
}

bool game_start()
{
	RandGen a;
	a.RandInt(1);

	apstring user_string, left_right_choice, c_final, u_final;
	int chars, comp_dir, real;
	bool choice, b_conv;

	cout << "Please input a string(no spaces): ";
	cin >> user_string;
	cout << "Remove from the left or right? ";
	cin >> left_right_choice;
	real = a.RandInt(0, 1);
	cout << "You must match this string: \"";
	double c_num = a.RandInt(1, (1.0 / 2.0) * (user_string.length()));
	int c_num_i = (int) c_num;
	if (real)
	{
		c_final = right(user_string, c_num_i);
		cout << c_final << '\"' << endl;
		cout << 'a' << c_final << c_num_i;
	}
	else
	{
		c_final = left(user_string, c_num_i);
		cout << c_final << '\"' << endl;
		cout << 'b' << c_final << c_num_i;
	}
	cout << "How many characters would you like to remove: ";
	cin >> chars;
	chars = user_string.length() - chars;
	cout << "0 or 1: ";
	cin >> choice;

	comp_dir = a.RandInt(0, 1);
	if (comp_dir == 0)
	{
		b_conv = false;
	}
	else if (comp_dir == 1)
	{
		b_conv = true;
	}
	else
	{
		return false;
	}

	if (b_conv == choice)
	{
		if (left_right_choice == "left")
		{
			u_final = right(user_string, chars);
			cout << 'a';
		}
		else
		{
			u_final = left(user_string, chars);
			cout << 'b';
		}
	}
	else
	{
		if (left_right_choice == "left")
		{
			u_final = left(user_string, chars);
			cout << 'c';
		}
		else
		{
			u_final = right(user_string, chars);
			cout << 'd';
		}
	}

	cout << "Your String: \"" << u_final << '\"' << endl;

	if (u_final == c_final)
	{
		return true;
	}
	else
	{
		return false;
	}
}