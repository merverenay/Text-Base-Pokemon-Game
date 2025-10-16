#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "Constants.hpp"
#include "Object.hpp"
#include "EquipmentSlots.hpp"
#include "Dice.hpp"

// static const int MAX_Y = 21;
// static const int MAX_X = 80;

class Player
{
public:
    int x, y;
    int speed;
    char prevTile;
    char memory[MAX_Y][MAX_X];

    int hp = 50; // NEW
    Dice baseDamage{0, 1, 4};
    // Dice baseDamage;

    // Object *equipment[12]; // a-l slots
    Object *equipment[SLOT_COUNT];

    Object *inventory[10]; // 0-9 slots

    Player() : x(0), y(0), speed(10), prevTile('.')
    {
        for (int y = 0; y < MAX_Y; ++y)
            for (int x = 0; x < MAX_X; ++x)
                memory[y][x] = ' ';
        for (int i = 0; i < 12; ++i)
            equipment[i] = nullptr;
        for (int i = 0; i < 10; ++i)
            inventory[i] = nullptr;
    }

    void place(int newX, int newY, char tile);
    bool isAlive() const;
    int getTotalSpeed() const;
    Dice getTotalDamage() const;
    int lastHpRegenTurn = 0; // Track last regen time
};

#endif
