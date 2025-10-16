#ifndef MONSTER_HPP
#define MONSTER_HPP

#include <string>
#include "Dice.hpp"

class Monster
{
public:
    int x, y;
    int speed;
    int hp;
    Dice damage;

    char type;
    std::string name;
    std::string color;
    std::string abilities;

    bool intelligence;
    bool telepathic;
    bool tunneling;
    bool erratic;

    int lastSeenX = -1;
    int lastSeenY = -1;
    char prevTile;

    Monster();
    void place(int newX, int newY);
};

#endif
