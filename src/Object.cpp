#include "Object.hpp"
#include <random>

Object::Object(const ObjectDescription &desc)
{
    name = desc.name;
    description = desc.description;
    symbol = desc.symbol;
    color = desc.color;
    hit = desc.hit.roll();
    dodge = desc.dodge.roll();
    def = desc.def.roll();
    weight = desc.weight.roll();
    speed = desc.speed.roll();
    attr = desc.attr.roll();
    val = desc.val.roll();
    damage = desc.dam;
    type = desc.type;
    artifact = desc.artifact;
}
