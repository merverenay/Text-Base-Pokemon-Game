#include "Player.hpp"
#include "EquipmentSlots.hpp"

void Player::place(int newX, int newY, char tile)
{
    x = newX;
    y = newY;
    prevTile = tile; // save original tile
}

bool Player::isAlive() const
{
    return x >= 0 && y >= 0;
}
int Player::getTotalSpeed() const
{
    int total = speed;
    for (int i = 0; i < SLOT_COUNT; ++i)
        if (equipment[i])
            total += equipment[i]->speed;
    return total;
}

Dice Player::getTotalDamage() const
{
    Dice total = (equipment[SLOT_WEAPON] ? Dice() : baseDamage); // base damage only if no weapon
    for (int i = 0; i < SLOT_COUNT; ++i)
        if (equipment[i])
            total.base += equipment[i]->damage.base,
                total.dice += equipment[i]->damage.dice,
                total.sides = std::max(total.sides, equipment[i]->damage.sides); // merge logic can vary
    return total;
}
