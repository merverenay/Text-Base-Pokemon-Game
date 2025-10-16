#include "Monster.hpp"

Monster::Monster() : x(0), y(0), speed(10), type('0'),
                     intelligence(false), telepathic(false), tunneling(false), erratic(false),
                     lastSeenX(-1), lastSeenY(-1) {}

void Monster::place(int newX, int newY)
{
    x = newX;
    y = newY;
}
