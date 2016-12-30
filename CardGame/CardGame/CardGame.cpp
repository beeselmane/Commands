#include <iostream>

#include "Deck.h"
#include "Hand.h"
#include "Card.h"

using std::cout;
using std::endl;
using std::cin;

#define DSIZE 52

int main(int argc, const char *const *argv)
{
    Deck deck(DSIZE);

    for (int i = 0; i < (DSIZE / 2); i++) cout << deck.deal().toString() << endl;
    deck.put(Card("Clubs", POWER_TWO));
    for (int i = 0; i < (DSIZE / 2) + 1; i++) cout << deck.deal().toString() << endl;

    return(0);
}
