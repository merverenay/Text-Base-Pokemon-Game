#include "Dungeon.hpp"
#include "SaveLoad.hpp"
#include "Event.hpp"
#include "MonsterParser.hpp"
#include "ObjectParser.hpp"

#include <ncurses.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>

int main(int argc, char *argv[])
{
    //     // curs_set(0);
    //     std::string filepath = std::string(getenv("HOME")) + "/.rlg327/dungeon";
    //     bool saveFlag = false, loadFlag = false;
    //     int numMonsters = 10;

    //     // Parse command-line arguments
    //     for (int i = 1; i < argc; ++i)
    //     {
    //         if (strcmp(argv[i], "--save") == 0)
    //         {
    //             saveFlag = true;
    //             if (i + 1 < argc && argv[i + 1][0] != '-')
    //             {
    //                 filepath = argv[++i];
    //             }
    //         }
    //         else if (strcmp(argv[i], "--load") == 0)
    //         {
    //             loadFlag = true;
    //             if (i + 1 < argc && argv[i + 1][0] != '-')
    //             {
    //                 filepath = argv[++i];
    //             }
    //         }
    //         else if (strcmp(argv[i], "--nummon") == 0 && i + 1 < argc)
    //         {
    //             numMonsters = std::atoi(argv[++i]);
    //             if (numMonsters <= 0 || numMonsters > MAX_MONSTERS)
    //             {
    //                 printf("Invalid number of monsters. Defaulting to 10.\n");
    //                 numMonsters = 10;
    //             }
    //         }
    //     }
    //     Dungeon dungeon;
    //     dungeon.numMonsters = numMonsters;

    //     initscr();
    //     raw();
    //     noecho();
    //     keypad(stdscr, TRUE);
    //     // curs_set(0);

    //     // Dungeon dungeon;

    //     if (loadFlag)
    //     {
    //         if (!SaveLoad::load(filepath, dungeon))
    //         {
    //             endwin();
    //             return 1;
    //         }
    //     }
    //     else
    //     {
    //         dungeon.generateRooms();
    //         dungeon.connectRooms();
    //         dungeon.placeStairs();
    //         dungeon.placePlayer();
    //         // dungeon.generateMonsters(numMonsters);
    //         dungeon.generateMonsters(dungeon.numMonsters);
    //         // dungeon.generateMonsters();
    //     }

    //     dungeon.printDungeon();
    //     dungeon.gameLoop();

    //     mvprintw(0, 0, "Press any key to exit...");
    //     refresh();
    //     getch();

    //     if (saveFlag)
    //     {
    //         SaveLoad::save(filepath, dungeon);
    //     }

    //     endwin();
    //     return 0;
    std::string filepath = std::string(getenv("HOME")) + "/.rlg327/dungeon";
    bool saveFlag = false, loadFlag = false, parseMonstersOnly = false;
    int numMonsters = 10;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--save") == 0)
        {
            saveFlag = true;
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                filepath = argv[++i];
            }
        }
        else if (strcmp(argv[i], "--load") == 0)
        {
            loadFlag = true;
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                filepath = argv[++i];
            }
        }
        else if (strcmp(argv[i], "--nummon") == 0 && i + 1 < argc)
        {
            numMonsters = std::atoi(argv[++i]);
            if (numMonsters <= 0 || numMonsters > MAX_MONSTERS)
            {
                std::cout << "Invalid number of monsters. Defaulting to 10.\n";
                numMonsters = 10;
            }
        }
        else if (strcmp(argv[i], "--parse-monsters") == 0)
        {
            parseMonstersOnly = true;
        }
    }

    // If --parse-monsters is passed, parse and print the monster definitions, then exit
    if (parseMonstersOnly)
    {
        std::string monsterPath = "monster_desc.txt";

        auto monsters = parseMonsterFile(monsterPath);

        for (const auto &m : monsters)
        {
            m.print(); // Print all fields as required
        }

        return 0; // Don't run the dungeon
    }

    // ---- Dungeon Game Normal Flow ----
    Dungeon dungeon;
    dungeon.monsterTemplates = parseMonsterFile("monster_desc.txt"); //////////////////////////////////////

    dungeon.numMonsters = numMonsters;

    initscr();
    start_color();
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    // Add more if needed

    raw();
    noecho();
    keypad(stdscr, TRUE);

    if (loadFlag)
    {
        if (!SaveLoad::load(filepath, dungeon))
        {
            endwin();
            return 1;
        }
    }
    else
    {
        dungeon.generateRooms();
        dungeon.connectRooms();
        dungeon.placeStairs();
        dungeon.placePlayer();
        dungeon.generateMonsters(dungeon.numMonsters);
        dungeon.objectTemplates = parseObjectFile("object_desc.txt");
        dungeon.generateObjects(10); ///////////////////////////////////////////////////////
    }

    dungeon.printDungeon();
    dungeon.gameLoop(); //////////////////////////////////////////////////////////////////////////////////////////////

    mvprintw(0, 0, "Press any key to exit...");
    refresh();
    getch();

    if (saveFlag)
    {
        SaveLoad::save(filepath, dungeon);
    }
    dungeon.clearObjects();
    endwin();
    return 0;
}
