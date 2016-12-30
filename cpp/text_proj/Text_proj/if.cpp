/**0.  Using a RandGen object, have the computer pick a random number between 1 and 10. Ask the user for 1 guess.
Tell them if they are right.
Tell them if they are too high or too low.
Tell them if they are hot (within 1 either way), warm (within three either way), or cold (more than 3 off)
1. One of the common tasks in maintaining data is to use the data to create a "form letter". We do this often at school. Ask the user for a first name, grade level, and gender (m/f). Once you have that data write the following:
To the Parent of [name]
Your [son/daughter] [name] is required to bring [his/her] iPad with [him/her] to the [freshman/sophomore/junior/senior] meeting.
The data in [] should be replaced with the appropriate personal pronoun, name, or class level.

2. The Pennsylvania Lottery! Two heads are better than one! Work with a partner to brainstorm and write a program to "play the lottery".
In this program you will ask the user for a number to "play". This number should be a single integer between 0 and 999. The user must enter numbers without leading 0s. If they want to play 097, they must enter it as 97. If you don't believe me try it!
Ask for "boxed" or "straight". Straight means their number must match the order picked (e.g. 375 matches 3 7 5) and they win $500 if they win straight. Boxed means that the digits chosen must be in their number in any order (e.g. 374 matches 7 3 4) and they win $90 if they win boxed.
Once chosen, the computer will pick its 3 numbers 1 digit at a time - just like on TV.
Tell whether they win or lose and how much money they win (if they win).
This should have functions galore! Divide them up with your partner to write. Here's what main() might look like
int main()
{
    bool isBoxed;
    int myNumber = askNumber();
    isBoxed = askBoxed();
    int lottery1, lottery2, lottery3
    lottery1 = pickOne();
    lottery2 = ...;
    lottery3 = ...;
    bool isWinner = checkStraight(lottery1,lottery2, lottery3, myNumber);
    if(isBoxed && !isWinner) isWinner= checkBoxed(lottery1, lottery2, lottery3, myNumber);
    if(isWinner)...
    return 0;
}*/
#include <iostream>
#include "S:\cpp\cpp\apstring.h"
#include "S:\cpp\cpp\RandGen.h"

using namespace std;

// Functions - 1
int getNum();
int getGuess();
void isClose(int *number, int *guess);

// Functions - 2


// Functions - 3


int main(void)
{
	// 1
	int guess = getGuess();
	int number = getNum();
	isClose((int *) number, (int *) guess);

	// 2

	// 3
	

	return(0);
}

int getNum()
{
	RandGen rand;
	rand.RandInt(1);
	return (rand.RandInt(1, 10));
}

int getGuess()
{
	int i;
	cout << "Guess a number between 1 and 10: " << endl;
	cin >> i; 
	return(i);
}

void isClose(int *number, int *guess)
{
	int x, y;
	x = (int) number;
	y = (int) guess;

	if (x == y)
	{
		cout << "You guessed it!!";
		return;
	}
	else if (x > y)
	{
		cout << "You are too low." << endl;
		return;
	}
	else if (x < y)
	{
		cout << "You are too high." << endl;
		if (y - x == 1)
		{
			cout << "You are only 1 away!!" << endl;
		}
		else if (y - x <= 3)
		{
			cout << "You are less than 3 away!!" << endl;
		}
		else
		{
			cout << "You are more than 3 away." << endl;
		}
	}
}