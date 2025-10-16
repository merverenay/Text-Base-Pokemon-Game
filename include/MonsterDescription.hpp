#ifndef MONSTER_DESCRIPTION_HPP
#define MONSTER_DESCRIPTION_HPP

#include <string>
#include "Dice.hpp"

class MonsterDescription
{
public:
    std::string name;
    std::string description;
    std::string color;
    Dice speed;
    Dice damage;
    Dice hp;
    std::string abilities;
    char symbol;
    int rarity;
    

    void print() const;
};

#endif
