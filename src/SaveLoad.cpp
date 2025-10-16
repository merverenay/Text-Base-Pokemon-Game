#include "SaveLoad.hpp"
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <vector>
#include <iostream>

void SaveLoad::save(const std::string &path, const Dungeon &dungeon)
{
    std::ofstream file(path, std::ios::binary);
    if (!file)
    {
        std::cerr << "ERROR: Cannot open file for saving.\n";
        return;
    }

    const char header[] = "RLG327-S2025";
    file.write(header, 12);

    uint32_t version = htonl(0);
    file.write(reinterpret_cast<char *>(&version), 4);

    uint32_t file_size = htonl(170); // Approximate size; fix as needed
    file.write(reinterpret_cast<char *>(&file_size), 4);

    uint8_t pc_x = dungeon.getPlayer().x;
    uint8_t pc_y = dungeon.getPlayer().y;
    file.write(reinterpret_cast<char *>(&pc_x), 1);
    file.write(reinterpret_cast<char *>(&pc_y), 1);

    // Write hardness
    for (int y = 0; y < MAX_Y; ++y)
    {
        for (int x = 0; x < MAX_X; ++x)
        {
            uint8_t h = dungeon.getHardness(y, x);
            file.write(reinterpret_cast<char *>(&h), 1);
        }
    }

    // Write rooms
    uint16_t num_rooms = htons(dungeon.getRooms().size());
    file.write(reinterpret_cast<char *>(&num_rooms), 2);
    for (const Room &r : dungeon.getRooms())
    {
        // uint8_t data[4] = {r.x, r.y, r.width, r.height};
        uint8_t data[4] = {
            static_cast<uint8_t>(r.x),
            static_cast<uint8_t>(r.y),
            static_cast<uint8_t>(r.width),
            static_cast<uint8_t>(r.height)};

        file.write(reinterpret_cast<char *>(data), 4);
    }

    file.close();
    std::cout << "Dungeon saved to " << path << "\n";
}

bool SaveLoad::load(const std::string &path, Dungeon &dungeon)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        std::cerr << "ERROR: File not found: " << path << "\n";
        return false;
    }

    char header[13] = {};
    file.read(header, 12);
    if (std::string(header) != "RLG327-S2025")
    {
        std::cerr << "ERROR: Invalid dungeon file format.\n";
        return false;
    }

    uint32_t version;
    file.read(reinterpret_cast<char *>(&version), 4);
    version = ntohl(version);

    if (version != 0)
    {
        std::cerr << "ERROR: Unsupported version.\n";
        return false;
    }

    uint32_t file_size;
    file.read(reinterpret_cast<char *>(&file_size), 4);
    file_size = ntohl(file_size);

    uint8_t pc_x, pc_y;
    file.read(reinterpret_cast<char *>(&pc_x), 1);
    file.read(reinterpret_cast<char *>(&pc_y), 1);
    char tileUnderPlayer = dungeon.getTile(pc_y, pc_x);
    dungeon.getPlayer().place(pc_x, pc_y, tileUnderPlayer);
    // dungeon.getPlayer().place(pc_x, pc_y);

    for (int y = 0; y < MAX_Y; ++y)
    {
        for (int x = 0; x < MAX_X; ++x)
        {
            uint8_t h;
            file.read(reinterpret_cast<char *>(&h), 1);
            dungeon.setHardness(y, x, h);
            dungeon.setTile(y, x, h == 0 ? '#' : ' ');
        }
    }

    uint16_t num_rooms;
    file.read(reinterpret_cast<char *>(&num_rooms), 2);
    num_rooms = ntohs(num_rooms);

    std::vector<Room> loadedRooms;
    for (int i = 0; i < num_rooms; ++i)
    {
        Room r;
        file.read(reinterpret_cast<char *>(&r.x), 1);
        file.read(reinterpret_cast<char *>(&r.y), 1);
        file.read(reinterpret_cast<char *>(&r.width), 1);
        file.read(reinterpret_cast<char *>(&r.height), 1);
        loadedRooms.push_back(r);
        for (int y = r.y; y < r.y + r.height; ++y)
            for (int x = r.x; x < r.x + r.width; ++x)
                dungeon.setTile(y, x, '.');
    }

    dungeon.setRooms(loadedRooms);
    dungeon.setTile(pc_y, pc_x, '@');

    file.close();
    std::cout << "Dungeon loaded from " << path << "\n";
    return true;
}
