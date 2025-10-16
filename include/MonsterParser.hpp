#ifndef MONSTER_PARSER_HPP
#define MONSTER_PARSER_HPP

#include <vector>
#include <string>
#include "MonsterDescription.hpp"

std::vector<MonsterDescription> parseMonsterFile(const std::string &filepath);

#endif
