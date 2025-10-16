#include "ObjectDescription.hpp"
#include "Object.hpp"

ObjectDescription::ObjectDescription() : artifact(false), rarity(0), type(ObjectType::NONE), symbol('*') {}

Object *ObjectDescription::generateInstance() const
{
    return new Object(*this);
}
