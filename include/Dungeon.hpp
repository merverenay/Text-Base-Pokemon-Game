#ifndef DUNGEON_HPP
#define DUNGEON_HPP

#include "Player.hpp"
#include "Monster.hpp"
#include "Room.hpp"
#include "Event.hpp"
#include "Constants.hpp"
#include "MonsterDescription.hpp"
#include "Object.hpp"

#include <unordered_set>

// #include "EventQueue.hpp"

#include <vector>
#include <cstdint>

// const int MAX_X = 80;
// const int MAX_Y = 21;
// const int MAX_ROOMS = 9;
// const int MIN_ROOMS = 6;
// const int MAX_MONSTERS = 20;

class Dungeon
{
public:
    char grid[MAX_Y][MAX_X];
    int hardness[MAX_Y][MAX_X];
    int nonTunnelingDist[MAX_Y][MAX_X];
    int tunnelingDist[MAX_Y][MAX_X];

    bool fogEnabled = false; // default to fog ON
    bool teleportMode = false;
    bool teleportRandom = false;
    int teleportCursorX = 0;
    int teleportCursorY = 0;

    bool regenerateFlag = false;
    int numMonsters = 10; //////////

    Player pc;
    std::vector<Monster> monsters;
    std::vector<Room> rooms;

    std::vector<ObjectDescription> objectTemplates;
    std::vector<Object *> objects;                      // active objects in dungeon
    std::unordered_set<std::string> generatedArtifacts; /////////////////////////////////////////////////////

    Dungeon();

    void init();
    void generateRooms();
    void connectRooms();
    void placeStairs();
    void placePlayer();
    void generateMonsters(int count);

    void computePathfinding();
    void printDistanceMap(int dist[MAX_Y][MAX_X]);
    void printDungeon();

    Player &getPlayer();
    const std::vector<Room> &getRooms() const;
    void setRooms(const std::vector<Room> &newRooms);

    char getTile(int y, int x) const;
    void setTile(int y, int x, char val);

    uint8_t getHardness(int y, int x) const;
    void setHardness(int y, int x, uint8_t val);

    void gameLoop();                 // Main game loop
    void processEvent(EventQueue &); // One event step
    void drawDungeon();
    bool movePlayer();            // PC input
    void moveMonster(Monster &m); // Monster behavior
    void showMonsterList();
    void computeDistances(bool tunneling);
    std::pair<int, int> getNextStepTowardPC(int x, int y, bool tunneling);
    bool canSeePC(const Monster &m);
    const Player &getPlayer() const;
    void drawCorridor(int x1, int y1, int x2, int y2);
    void regenerateDungeon();
    std::vector<MonsterDescription> monsterTemplates; /////////////////////////////////////////////
    void generateObjects(int count);
    void drawObjects(); // for rendering
    void clearObjects();
    void attack(Monster &m);
    void attackPC(Monster &m);
    int currentTurn = 0;
};

#endif