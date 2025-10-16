#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include "Dice.hpp"
#include "ObjectDescription.hpp"

class Object
{
public:
    std::string name;
    std::string description;
    char symbol;
    std::string color;
    int hit, dodge, def, weight, speed, attr, val;
    Dice damage;
    ObjectType type;
    bool artifact;

    int x, y; // Dungeon position

    Object(const ObjectDescription &desc);
};
inline std::string objectTypeName(ObjectType type)
{
    switch (type)
    {
    case ObjectType::WEAPON:
        return "WEAPON";
    case ObjectType::OFFHAND:
        return "OFFHAND";
    case ObjectType::RANGED:
        return "RANGED";
    case ObjectType::ARMOR:
        return "ARMOR";
    case ObjectType::HELMET:
        return "HELMET";
    case ObjectType::CLOAK:
        return "CLOAK";
    case ObjectType::GLOVES:
        return "GLOVES";
    case ObjectType::BOOTS:
        return "BOOTS";
    case ObjectType::AMULET:
        return "AMULET";
    case ObjectType::LIGHT:
        return "LIGHT";
    case ObjectType::RING:
        return "RING";
    default:
        return "UNKNOWN";
    }
}

#endif
