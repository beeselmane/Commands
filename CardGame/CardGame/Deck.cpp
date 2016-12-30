#include <assert.h>
#include <cstdlib>
#include "Deck.h"

using std::rand;

#define NPOW 15
#define SPOW 2

#define fore(x, y, i) for (int i = x; i < y; i++)

static int randmax(int max)
{
    int r = rand();
    return (r % (max + 1));
}

template<typename T> void swap(T *a, T *b)
{
    T c = (*a);
    (*a) = (*b);
    (*b) = c;
}

Deck::Deck(int csize)
{
    this->cards = vector<Card>(csize);
    this->size = csize;

    assert(this->size == 52);
    string suits[4] = {"Hearts", "Diamonds", "Clubs", "Spades"};

    fore(SPOW, NPOW, i) fore(0, 4, j) this->cards.push_back(Card(suits[j], i));
}

// Nothing to do here
Deck::~Deck() {}

Card Deck::deal()
{
    Card c = this->cards.back();
    this->cards.pop_back();
    return c;
}

void Deck::put(Card card)
{
    this->cards.insert(this->cards.begin(), card);
}

void Deck::shuffle()
{
    fore(0, (this->size * 4), i)
    {
        swap<Card>(&this->cards[randmax(this->size)], &this->cards[randmax(this->size)]);
    }
}

int Deck::getSize()
{
    return this->size;
}
