#ifndef OBJECT_DESCRIPTION_HPP
#define OBJECT_DESCRIPTION_HPP

#include <string>
#include "Dice.hpp"

enum class ObjectType
{
    WEAPON,
    OFFHAND,
    RANGED,
    ARMOR,
    HELMET,
    CLOAK,
    GLOVES,
    BOOTS,
    RING,
    AMULET,
    LIGHT,
    SCROLL,
    BOOK,
    FLASK,
    GOLD,
    AMMUNITION,
    FOOD,
    WAND,
    CONTAINER,
    STACK,
    NONE
};

class Object;

class ObjectDescription
{
public:
    std::string name, description, color;
    Dice hit, dodge, def, weight, speed, attr, val, dam;
    ObjectType type;
    bool artifact;
    int rarity;
    char symbol;

    ObjectDescription();

    void print() const;
    Object *generateInstance() const;
    // std::string description;
};

#endif
