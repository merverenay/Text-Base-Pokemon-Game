#ifndef SAVELOAD_HPP
#define SAVELOAD_HPP

#include "Room.hpp"
#include "Player.hpp"
#include "Monster.hpp"
#include "Dungeon.hpp"
#include <string>

class SaveLoad
{
public:
    static void save(const std::string &filepath, const Dungeon &dungeon);
    static bool load(const std::string &filepath, Dungeon &dungeon);
};

#endif
