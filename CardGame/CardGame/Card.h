#ifndef __CardGame__Card__
#define __CardGame__Card__

#define POWER_TWO   2
#define POWER_THREE 3
#define POWER_FOUR  4
#define POWER_FIVE  5
#define POWER_SIX   6
#define POWER_SEVEN 7
#define POWER_EIGHT 8
#define POWER_NINE  9
#define POWER_TEN   10
#define POWER_JACK  11
#define POWER_QUEEN 12
#define POWER_KING  13

#include <string>

using std::ostream;
using std::string;

// Immutable Card Class

class Card
{
public:
    Card(string suit, int power);
    Card(); // Invalid Constructor
    ~Card();

    string getName();
    string getSuit();
    int getPower();

    string toString();

    Card operator =  (Card other);
    bool operator == (Card other);
    bool operator >  (Card other);
    bool operator <  (Card other);
    bool operator >= (Card other);
    bool operator <= (Card other);

private:
    string suit;
    int power;
};

#endif /* defined(__CardGame__Card__) */
