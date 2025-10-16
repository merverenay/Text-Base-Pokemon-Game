#include "ObjectDescription.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>

std::vector<ObjectDescription> parseObjectFile(const std::string &filepath)
{
    std::ifstream file(filepath);
    std::vector<ObjectDescription> objects;
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "Cannot open " << filepath << "\n";
        return objects;
    }

    getline(file, line);
    if (line != "RLG327 OBJECT DESCRIPTION 1")
    {
        std::cerr << "Invalid file header\n";
        return objects;
    }

    while (getline(file, line))
    {
        if (line == "BEGIN OBJECT")
        {
            ObjectDescription od;
            std::string desc;
            std::set<std::string> seen;

            while (getline(file, line) && line != "END")
            {
                std::istringstream iss(line);
                std::string keyword;
                iss >> keyword;

                if (keyword == "NAME")
                {
                    getline(iss, od.name);
                    od.name = od.name.substr(1);
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
                    getline(iss, od.color);
                    od.color = od.color.substr(1);
                    seen.insert("COLOR");
                }
                else if (keyword == "SYMB")
                {
                    iss >> od.symbol;
                    seen.insert("SYMB");
                }
                else if (keyword == "TYPE")
                {
                    std::string t;
                    iss >> t;
                    if (t == "WEAPON")
                        od.type = ObjectType::WEAPON;
                    else if (t == "FLASK")
                        od.type = ObjectType::FLASK;
                    else if (t == "RING")
                        od.type = ObjectType::RING;
                    else
                        od.type = ObjectType::NONE;
                    seen.insert("TYPE");
                }
                else if (keyword == "HIT")
                {
                    std::string val;
                    iss >> val;
                    sscanf(val.c_str(), "%d+%dd%d", &od.hit.base, &od.hit.dice, &od.hit.sides);
                    seen.insert("HIT");
                }
                else if (keyword == "DODGE")
                {
                    std::string val;
                    iss >> val;
                    sscanf(val.c_str(), "%d+%dd%d", &od.dodge.base, &od.dodge.dice, &od.dodge.sides);
                    seen.insert("DODGE");
                }
                else if (keyword == "DEF")
                {
                    std::string val;
                    iss >> val;
                    sscanf(val.c_str(), "%d+%dd%d", &od.def.base, &od.def.dice, &od.def.sides);
                    seen.insert("DEF");
                }
                else if (keyword == "WEIGHT")
                {
                    std::string val;
                    iss >> val;
                    sscanf(val.c_str(), "%d+%dd%d", &od.weight.base, &od.weight.dice, &od.weight.sides);
                    seen.insert("WEIGHT");
                }
                else if (keyword == "SPEED")
                {
                    std::string val;
                    iss >> val;
                    sscanf(val.c_str(), "%d+%dd%d", &od.speed.base, &od.speed.dice, &od.speed.sides);
                    seen.insert("SPEED");
                }
                else if (keyword == "ATTR")
                {
                    std::string val;
                    iss >> val;
                    sscanf(val.c_str(), "%d+%dd%d", &od.attr.base, &od.attr.dice, &od.attr.sides);
                    seen.insert("ATTR");
                }
                else if (keyword == "VAL")
                {
                    std::string val;
                    iss >> val;
                    sscanf(val.c_str(), "%d+%dd%d", &od.val.base, &od.val.dice, &od.val.sides);
                    seen.insert("VAL");
                }
                else if (keyword == "DAM")
                {
                    std::string val;
                    iss >> val;
                    sscanf(val.c_str(), "%d+%dd%d", &od.dam.base, &od.dam.dice, &od.dam.sides);
                    seen.insert("DAM");
                }
                else if (keyword == "ART")
                {
                    int art;
                    iss >> art;
                    od.artifact = (art != 0);
                    seen.insert("ART");
                }
                else if (keyword == "RRTY")
                {
                    iss >> od.rarity;
                    seen.insert("RRTY");
                }
            }

            if (seen.size() >= 12)
            {
                od.description = desc;
                objects.push_back(od);
            }
        }
    }

    return objects;
}
