#ifndef __CardGame__Hand__
#define __CardGame__Hand__

#include <vector>
#include "Card.h"

using std::vector;

class Hand
{
public:
    Hand(int size);
    ~Hand();

    //

private:
    vector<Card> cards;
    int size;
};

#endif /* defined(__CardGame__Hand__) */
