#include <iostream>
using namespace std;

int getNum();
int getSum(int first, int second, int third);
void numOut(int sum);

int main(void)
{
	int x, y, z;
	x = getNum();
	y = getNum();
	z = getNum();
	numOut(getSum(x, y, z));
	return 0;
}

int getNum()
{
	int num;
	cout << "Please Input a Number: ";
	cin >> num;
	return (num);
}

int getSum(int x, int y, int z)
{
	return (x + y + z);
}

void numOut(int x)
{
	cout << "Your Number Is: " << x << endl;
	return;
}