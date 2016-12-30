#include <iostream>
#include "S:\cpp\apstring.h"
#include "S:\cpp\randgen.h"

using namespace std;

int main(void)
{
	RandGen rand;
	rand.RandInt(1);

	int user_number;
	int num1;

	cout << "Please Input a number 0-999: ";
	cin >> user_number;
	cout << "Computer's Number: ";

	for ( int i = 3 ; i != 0 ; i-- )
	{
		num1 = rand.RandInt(10);
		cout << num1;
	}

	cout << endl;
	cout << "Your Number: " << user_number << endl;
	cin.ignore();
	cin.ignore();

	// Next Assignment...

	apstring word;

	cout << "Please Input a Word" << endl;
	getline(cin, word);

	cout << "Length: " << word.length() << endl;
	cout << "First Letter: " << word[0] << endl;
	cout << "Last Letter: " << word[word.length() - 1] << endl;
	cout << "Middle: " << word.substr(1, (word.length() - 2)) << endl;
	cin.ignore();

	// Next Assignment...

	apstring first, middle, last;

	cout << "Please Input your names:" << endl;

	cout << "First: ";
	cin >> first;

	cout << "Middle: ";
	cin >> middle;

	cout << "Last: ";
	cin >> last;

	cout << first
		<< " " << middle
		<< " " << last
		<< endl << last
		<< ", " << first
		<< endl << last
		<< ", " << first[0]
		<< ". " << middle[0]
		<< ". " << endl;

		cin.ignore();
		cin.ignore();

	// Next Assignment...

	apstring first_name, last_name, username;

	first_name = first;
	last_name = last;

	cout << "Name: " << first << " " << last << endl;

	if (last_name.length() <= 5)
	{
		username = last_name;
		username += first_name.substr(0, (7 - last_name.length()));
		username += ((char) rand.RandInt(2, 9) + '0');
	}
	else
	{
		username = last_name.substr(0, 5);
		username += first_name.substr(0, 2);

		if (username.length() != 7)
		{
			cout << "An Error Occured... Please Try Again Later" << endl;
			return 1;
		}
		else
		{
			username += ((char) rand.RandInt(2, 9) + '0');
		}
	}

	cout << "Username: " << username << endl;
	cout << "Password: " << rand.RandInt(10000000) << endl;
	cout << '\n';

	return 0;
}