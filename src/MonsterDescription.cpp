#include "MonsterDescription.hpp"
#include <iostream>

void MonsterDescription::print() const
{
    std::cout << name << "\n"
              << description
              << symbol << "\n"
              << color << "\n"
              << speed.toString() << "\n"
              << abilities << "\n"
              << hp.toString() << "\n"
              << damage.toString() << "\n"
              << rarity << "\n\n";
}
