#include <iostream>
#include "S:\cpp\apstring.h"

using namespace std;

apstring getName();
int getAge();
int getGrade();
apstring getPhone();
void concat_dat(int, int, apstring, apstring);

int main(void)
{
	int age, grade;
	apstring name, phone;

	name = getName();
	age = getAge();
	grade = getGrade();
	phone = getPhone();

	concat_dat(age, grade, phone, name);
	cin.ignore();
	return 0;
}

apstring getName()
{
	apstring str;
	cout << "Please Input your name: ";
	getline(cin, str);
	return str;
}

int getAge()
{
	int inti;
	cout << "Please Input your age: ";
	cin >> inti;
	return inti;
}

int getGrade()
{
	int inti;
	cout << "Please Input your grade: ";
	cin >> inti;
	return inti;
}

apstring getPhone()
{
	apstring phone;
	cout << "Please Input your phone number: ";
	cin >> phone;
	return phone;
}

void concat_dat(int age, int grade, apstring phone, apstring name)
{
	cout << "Grade: " << grade
		<< " - " << name
		<< " - " << "Age: "
		<< age << " - " << "Phone: "
		<< phone << endl;
}