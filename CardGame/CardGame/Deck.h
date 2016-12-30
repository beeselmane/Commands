#ifndef __CardGame__Deck__
#define __CardGame__Deck__ 1

#include <vector>
#include "Card.h"

using std::vector;

class Deck
{
public:
    Deck(int size);
    ~Deck();

    Card deal();
    void put(Card card);
    void shuffle();
    int getSize();

private:
    vector<Card> cards;
    int size;
};

#endif /* defined(__CardGame__Deck__) */
