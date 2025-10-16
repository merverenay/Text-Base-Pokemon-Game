#include "MonsterParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <cstdlib>
#include <cstring>

std::vector<MonsterDescription> parseMonsterFile(const std::string &filepath)
{
    std::ifstream file(filepath);
    std::vector<MonsterDescription> monsters;
    std::string line;
    bool headerFound = false;

    if (!file.is_open())
    {
        std::cerr << "Cannot open " << filepath << "\n";
        return monsters;
    }

    getline(file, line);
    if (line != "RLG327 MONSTER DESCRIPTION 1")
    {
        std::cerr << "Invalid file header\n";
        exit(1);
    }

    while (getline(file, line))
    {
        if (line == "BEGIN MONSTER")
        {
            MonsterDescription md;
            std::string desc;
            std::set<std::string> seen;
            while (getline(file, line) && line != "END")
            {
                std::istringstream iss(line);
                std::string keyword;
                iss >> keyword;

                if (keyword == "NAME")
                {
                    getline(iss, md.name);
                    md.name = md.name.substr(1);
                    seen.insert("NAME");
                }
                else if (keyword == "DESC")
                {
                    std::string d;
                    while (getline(file, d) && d != ".")
                        desc += d + "\n";
                    seen.insert("DESC");
                }
                else if (keyword == "COLOR")
                {
                    getline(iss, md.color);
                    md.color = md.color.substr(1);
                    seen.insert("COLOR");
                }
                else if (keyword == "SYMB")
                {
                    iss >> md.symbol;
                    seen.insert("SYMB");
                }
                else if (keyword == "SPEED")
                {
                    std::string val;
                    iss >> val;
                    sscanf(val.c_str(), "%d+%dd%d", &md.speed.base, &md.speed.dice, &md.speed.sides);
                    seen.insert("SPEED");
                }
                else if (keyword == "DAM")
                {
                    std::string val;
                    iss >> val;
                    sscanf(val.c_str(), "%d+%dd%d", &md.damage.base, &md.damage.dice, &md.damage.sides);
                    seen.insert("DAM");
                }
                else if (keyword == "HP")
                {
                    std::string val;
                    iss >> val;
                    sscanf(val.c_str(), "%d+%dd%d", &md.hp.base, &md.hp.dice, &md.hp.sides);
                    seen.insert("HP");
                }
                else if (keyword == "RRTY")
                {
                    iss >> md.rarity;
                    seen.insert("RRTY");
                }
                else if (keyword == "ABIL")
                {
                    std::string ab;
                    while (iss >> ab)
                        md.abilities += ab + " ";
                    if (!md.abilities.empty())
                        md.abilities.pop_back(); // remove last space
                    seen.insert("ABIL");
                }
                else
                {
                    seen.clear(); // invalid keyword
                    break;
                }
            }

            if (seen.size() == 9)
            {
                md.description = desc;
                monsters.push_back(md);
            }
        }
    }

    return monsters;
}
