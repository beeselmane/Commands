#include <cstdio>
#include "Card.h"

static string names[14] = {"Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace"};

Card::Card(string csuit, int cpower)
{
    this->power = cpower;
    this->suit = csuit;
}

Card::Card()
{
    this->power = 0;
    this->suit = -1;
}

// Nothing to do here
Card::~Card() {}

int Card::getPower()
{
    return this->power;
}

string Card::getName()
{
    return string(names[this->power - 1]);
}

string Card::getSuit()
{
    return this->suit;
}

Card Card::operator = (Card other)
{
    this->power = other.power;
    this->suit = other.suit;

    return *this;
}

bool Card::operator == (Card other)
{
    return ((this->power == other.power) &&
            (this->suit  == other.suit));
}

bool Card::operator < (Card other)
{
    return (this->power < other.power);
}

bool Card::operator > (Card other)
{
    return (this->power > other.power);
}

bool Card::operator >= (Card other)
{
    return ((*this) > other) || ((*this) == other);
}

bool Card::operator<=(Card other)
{
    return ((*this) < other) || ((*this) > other);
}

string Card::toString()
{
    char *string;
    asprintf(&string, "%s of %s", this->getName().c_str(), this->suit.c_str());
    return string;
}
