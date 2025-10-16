
#include "Event.hpp"
#include "Dungeon.hpp"

#include "Player.hpp"
#include "Object.hpp"
#include "ObjectDescription.hpp"

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <ncurses.h>
#include <algorithm>
#include <cstring> // for memcpy

Dungeon::Dungeon()
{
    srand(time(nullptr));

    init();
}

void Dungeon::init()
{
    for (int y = 0; y < MAX_Y; ++y)
    {
        for (int x = 0; x < MAX_X; ++x)
        {
            grid[y][x] = ' ';
            hardness[y][x] = rand() % 254 + 1;
        }
    }
    for (int x = 0; x < MAX_X; ++x)
    {
        grid[0][x] = '-';
        grid[MAX_Y - 1][x] = '-';
        hardness[0][x] = 255;
        hardness[MAX_Y - 1][x] = 255;
    }
    for (int y = 0; y < MAX_Y; ++y)
    {
        grid[y][0] = '|';
        grid[y][MAX_X - 1] = '|';
        hardness[y][0] = 255;
        hardness[y][MAX_X - 1] = 255;
    }
}
int objectTypeToEquipSlot(ObjectType type, const Player &pc)
{
    switch (type)
    {
    case ObjectType::WEAPON:
        return SLOT_WEAPON;
    case ObjectType::OFFHAND:
        return SLOT_OFFHAND;
    case ObjectType::RANGED:
        return SLOT_RANGED;
    case ObjectType::ARMOR:
        return SLOT_ARMOR;
    case ObjectType::HELMET:
        return SLOT_HELMET;
    case ObjectType::CLOAK:
        return SLOT_CLOAK;
    case ObjectType::GLOVES:
        return SLOT_GLOVES;
    case ObjectType::BOOTS:
        return SLOT_BOOTS;
    case ObjectType::AMULET:
        return SLOT_AMULET;
    case ObjectType::LIGHT:
        return SLOT_LIGHT;
    case ObjectType::RING:
        return (pc.equipment[SLOT_RING1] ? SLOT_RING2 : SLOT_RING1);
    default:
        return -1;
    }
}

short getColorFromString(const std::string &color)
{
    if (color == "RED")
        return COLOR_RED;
    if (color == "GREEN")
        return COLOR_GREEN;
    if (color == "BLUE")
        return COLOR_BLUE;
    if (color == "CYAN")
        return COLOR_CYAN;
    if (color == "YELLOW")
        return COLOR_YELLOW;
    if (color == "MAGENTA")
        return COLOR_MAGENTA;
    if (color == "WHITE")
        return COLOR_WHITE;
    if (color == "BLACK")
        return COLOR_WHITE; // fallback to WHITE since BLACK is invisible
    return COLOR_WHITE;     // default fallback
}

void Dungeon::generateRooms()
{
    int roomsToPlace = rand() % (MAX_ROOMS - MIN_ROOMS + 1) + MIN_ROOMS;
    int failedAttempts = 0;
    rooms.clear();

    while ((int)rooms.size() < roomsToPlace && failedAttempts < 2000)
    {
        Room r;
        r.width = rand() % 8 + 4;
        r.height = rand() % 6 + 3;
        r.x = rand() % (MAX_X - r.width - 2) + 1;
        r.y = rand() % (MAX_Y - r.height - 2) + 1;

        bool fits = true;
        for (int y = r.y - 1; y <= r.y + r.height && fits; ++y)
        {
            for (int x = r.x - 1; x <= r.x + r.width; ++x)
            {
                if (grid[y][x] == '.')
                {
                    fits = false;
                    break;
                }
            }
        }

        if (fits)
        {
            for (int y = r.y; y < r.y + r.height; ++y)
            {
                for (int x = r.x; x < r.x + r.width; ++x)
                {
                    grid[y][x] = '.';
                    hardness[y][x] = 0;
                }
            }
            rooms.push_back(r);
        }
        else
        {
            ++failedAttempts;
        }
    }
}

void Dungeon::placePlayer()
{
    if (rooms.empty())
        return;

    Room r = rooms[rand() % rooms.size()];
    int px = r.x + rand() % r.width;
    int py = r.y + rand() % r.height;
    pc.place(px, py, grid[py][px]); // store the original tile beneath the player
}

// void Dungeon::generateMonsters(int count)
// {
//     // if (count == -1)
//     //     count = numMonsters;////////////////
//     monsters.clear();
//     for (int i = 0; i < count; ++i)
//     {
//         Monster m;
//         m.type = "0123456789abcdef"[rand() % 16];
//         m.speed = 5 + rand() % 16;
//         m.intelligence = rand() % 2;
//         m.telepathic = rand() % 2;
//         m.tunneling = rand() % 2;
//         m.erratic = rand() % 2;

//         int attempts = 0;
//         while (attempts++ < 100)
//         {
//             Room r = rooms[rand() % rooms.size()];
//             int mx = r.x + rand() % r.width;
//             int my = r.y + rand() % r.height;
//             if (grid[my][mx] == '.' && !(mx == pc.x && my == pc.y))
//             {
//                 m.place(mx, my);
//                 grid[my][mx] = m.type;
//                 monsters.push_back(m);
//                 break;
//             }
//         }
//     }
// }
void Dungeon::generateMonsters(int count)
{
    monsters.clear();

    if (monsterTemplates.empty())
    {
        mvprintw(MAX_Y, 0, "⚠️ No monster templates loaded!\n");
        refresh();
        return;
    }

    for (int i = 0; i < count; ++i)
    {
        // Pick a random template based on rarity
        const MonsterDescription *chosen = nullptr;
        for (int attempt = 0; attempt < 1000; ++attempt)
        {
            const MonsterDescription &candidate = monsterTemplates[rand() % monsterTemplates.size()];
            if (rand() % 100 < candidate.rarity)
            {
                chosen = &candidate;
                break;
            }
        }

        if (!chosen)
            continue; // skip if nothing valid

        Monster m;
        m.type = chosen->symbol;
        m.color = chosen->color; // SET THE MONSTER COLOR STRING
        m.speed = chosen->speed.roll();
        m.hp = chosen->hp.roll();
        m.damage = chosen->damage; // store Dice object for later use

        m.intelligence = chosen->abilities.find("SMART") != std::string::npos;
        m.telepathic = chosen->abilities.find("TELE") != std::string::npos;
        m.tunneling = chosen->abilities.find("TUNNEL") != std::string::npos;
        m.erratic = chosen->abilities.find("ERRATIC") != std::string::npos;
        // You can add more ability flags as needed

        // Place monster on a valid floor tile
        int attempts = 0;
        while (attempts++ < 100)
        {
            Room r = rooms[rand() % rooms.size()];
            int mx = r.x + rand() % r.width;
            int my = r.y + rand() % r.height;

            if (grid[my][mx] == '.' && !(mx == pc.x && my == pc.y))
            {
                m.place(mx, my);
                m.prevTile = grid[my][mx];
                grid[my][mx] = m.type;
                monsters.push_back(m);
                mvprintw(MAX_Y + i, 0, "Generated: %c with speed %d", m.type, m.speed);
                refresh();
                break;
            }
        }
    }
}

void Dungeon::printDungeon()
{
    for (int y = 0; y < MAX_Y; ++y)
    {
        for (int x = 0; x < MAX_X; ++x)
        {
            putchar(grid[y][x]);
        }
        putchar('\n');
    }
}

void Dungeon::printDistanceMap(int dist[MAX_Y][MAX_X])
{
    for (int y = 0; y < MAX_Y; ++y)
    {
        for (int x = 0; x < MAX_X; ++x)
        {
            if (grid[y][x] == '@')
                putchar('@');
            else if (dist[y][x] == INT_MAX)
                putchar(' ');
            else
                printf("%1d", dist[y][x] % 10);
        }
        putchar('\n');
    }
}

void Dungeon::connectRooms()
{
    int connected[MAX_ROOMS] = {0};
    connected[0] = 1;

    for (size_t i = 1; i < rooms.size(); ++i)
    {
        int closest = -1, minDist = INT_MAX;

        for (size_t j = 0; j < rooms.size(); ++j)
        {
            if (connected[j])
            {
                int dx = rooms[i].x - rooms[j].x;
                int dy = rooms[i].y - rooms[j].y;
                int dist = dx * dx + dy * dy;
                if (dist < minDist)
                {
                    minDist = dist;
                    closest = j;
                }
            }
        }

        if (closest != -1)
        {
            int x1 = rooms[i].x + rooms[i].width / 2;
            int y1 = rooms[i].y + rooms[i].height / 2;
            int x2 = rooms[closest].x + rooms[closest].width / 2;
            int y2 = rooms[closest].y + rooms[closest].height / 2;

            drawCorridor(x1, y1, x2, y2);
        }

        connected[i] = 1;
    }
}

void Dungeon::placeStairs()
{
    int numUp = rand() % 3 + 1;
    int numDown = rand() % 3 + 1;
    for (int i = 0; i < numUp + numDown; ++i)
    {
        char stair = (i < numUp) ? '<' : '>';
        Room r = rooms[rand() % rooms.size()];
        int sx = r.x + rand() % r.width;
        int sy = r.y + rand() % r.height;
        if (grid[sy][sx] == '.')
            grid[sy][sx] = stair;
    }
}

Player &Dungeon::getPlayer()
{
    return pc;
}

const std::vector<Room> &Dungeon::getRooms() const
{
    return rooms;
}

void Dungeon::setRooms(const std::vector<Room> &newRooms)
{
    rooms = newRooms;
}

char Dungeon::getTile(int y, int x) const
{
    return grid[y][x];
}

void Dungeon::setTile(int y, int x, char val)
{
    grid[y][x] = val;
}

uint8_t Dungeon::getHardness(int y, int x) const
{
    return hardness[y][x];
}

void Dungeon::setHardness(int y, int x, uint8_t val)
{
    hardness[y][x] = val;
}
void Dungeon::gameLoop()
{
    // generateMonsters(MAX_MONSTERS);
    EventQueue queue;

    // Push all monsters
    for (auto &m : monsters)
    {
        queue.push(Event(1000 / m.speed, &m));
    }

    // Push PC
    queue.push(Event(1000 / pc.speed, nullptr, true));

    while (pc.x >= 0 && pc.y >= 0)
    {
        processEvent(queue);
        drawDungeon(); // optional/////////////////////////////////////////////////////////////////////////////
        // printDungeon();
        if (regenerateFlag)
        {
            regenerateFlag = false;

            // Reset and rebuild queue after regen
            queue = EventQueue();
            for (auto &m : monsters)
                queue.push(Event(1000 / m.speed, &m));
            queue.push(Event(1000 / pc.speed, nullptr, true));
        }
    }

    mvprintw(MAX_Y + 1, 0, "Game Over. Press any key.");
    refresh();
    getch();
}

// void Dungeon::drawDungeon()
// {
//     clear();

//     for (int y = 0; y < MAX_Y; ++y)
//     {
//         for (int x = 0; x < MAX_X; ++x)
//         {
//             char ch;

//             if (!fogEnabled)
//             {
//                 // No fog: draw actual grid
//                 ch = grid[y][x];
//             }
//             else
//             {
//                 bool inLight = std::abs(pc.x - x) <= 2 && std::abs(pc.y - y) <= 2;

//                 if (inLight)
//                 {
//                     pc.memory[y][x] = grid[y][x]; // 💾 remember it
//                     ch = grid[y][x];
//                 }
//                 else
//                 {
//                     ch = pc.memory[y][x];
//                     if (ch == ' ')
//                         ch = ' '; // unseen
//                 }
//             }

//             // PC always rendered explicitly
//             if (x == pc.x && y == pc.y)
//                 ch = '@';

//             mvaddch(y, x, ch);
//         }
//     }

//     if (teleportMode)
//     {
//         mvaddch(teleportCursorY, teleportCursorX, '*');
//     }

//     move(pc.y, pc.x); // Set cursor
//     refresh();
// }
void Dungeon::drawDungeon()
{
    clear();

    for (int y = 0; y < MAX_Y; ++y)
    {
        for (int x = 0; x < MAX_X; ++x)
        {
            char ch;
            bool inLight = !fogEnabled || (std::abs(pc.x - x) <= 2 && std::abs(pc.y - y) <= 2);

            if (fogEnabled && inLight)
                pc.memory[y][x] = grid[y][x];

            // Default to memory if fog is on, otherwise actual grid
            ch = fogEnabled ? pc.memory[y][x] : grid[y][x];

            // Render PC
            if (x == pc.x && y == pc.y)
            {
                mvaddch(y, x, '@');
                continue;
            }

            // Render monster if present
            bool monsterHere = false;
            for (const auto &m : monsters)
            {
                if (m.x == x && m.y == y)
                {
                    // attron(COLOR_PAIR(COLOR_RED)); // example: monster color
                    // mvaddch(y, x, m.type);
                    // attroff(COLOR_PAIR(COLOR_RED));
                    short colorID = getColorFromString(m.color);
                    attron(COLOR_PAIR(colorID));
                    mvaddch(y, x, m.type);
                    attroff(COLOR_PAIR(colorID));

                    monsterHere = true;
                    break;
                }
            }
            if (monsterHere)
                continue;

            // Render object if present
            for (const auto &obj : objects)
            {
                if (obj->x == x && obj->y == y)
                {
                    short colorID = getColorFromString(obj->color);
                    attron(COLOR_PAIR(colorID));
                    mvaddch(y, x, obj->symbol);
                    attroff(COLOR_PAIR(colorID));
                    goto skip_tile_draw;
                }
            }

            // Render terrain
            mvaddch(y, x, ch);
        skip_tile_draw:;
        }
    }

    if (teleportMode)
    {
        mvaddch(teleportCursorY, teleportCursorX, '*');
    }

    move(pc.y, pc.x);
    refresh();
}

void Dungeon::processEvent(EventQueue &queue)
{
    if (queue.empty())
        return;

    Event e = queue.pop();

    currentTurn++; // Increment turn counter

    if (pc.hp > 0 && pc.hp < 50 && currentTurn - pc.lastHpRegenTurn >= 20)
    {
        pc.hp++;
        pc.lastHpRegenTurn = currentTurn;

        mvprintw(MAX_Y, 0, "You regenerated 1 HP. [Current HP: %d]  ", pc.hp);
        refresh();
        getch();
    }

    if (e.isPlayer)
    {
        if (movePlayer())
            e.time += 1000 / pc.speed;

        queue.push(e);
    }
    else if (e.monster)
    {
        moveMonster(*e.monster);
        e.time += 1000 / e.monster->speed;
        queue.push(e);
    }
}

void Dungeon::attack(Monster &m) ///////////////////////////////
{
    int totalDamage = pc.baseDamage.roll();

    for (int i = 0; i < SLOT_COUNT; ++i)
    {
        if (pc.equipment[i])
        {
            totalDamage += pc.equipment[i]->damage.roll();
        }
    }

    m.hp -= totalDamage;

    mvprintw(MAX_Y, 0, "You hit '%c' for %d damage. [HP left: %d]", m.type, totalDamage, m.hp);
    refresh();
    getch();

    if (m.hp <= 0)
    {
        if (m.name == "SpongeBob SquarePants")
        {
            mvprintw(MAX_Y + 1, 0, "🎉 You defeated the BOSS! You win!");
            refresh();
            getch();
            exit(0); // Game over - WIN
        }

        // Remove monster from grid
        grid[m.y][m.x] = '.';
        m.x = -1;
        m.y = -1;
    }
}
void Dungeon::attackPC(Monster &m)
{
    int damage = m.damage.roll();
    pc.hp -= damage;

    mvprintw(MAX_Y, 0, "'%c' hits you for %d damage. [Your HP: %d]", m.type, damage, pc.hp);
    refresh();
    getch();

    if (pc.hp <= 0)
    {
        mvprintw(MAX_Y + 1, 0, "💀 You died. Game Over.");
        refresh();
        getch();
        pc.place(-1, -1, pc.prevTile); // Trigger game loop end
    }
}

bool Dungeon::movePlayer()
{
    int ch = getch();
    int dx = 0, dy = 0;

    switch (ch)
    {
    case 'f':
        fogEnabled = !fogEnabled;
        drawDungeon();
        return false;

    case 'g':
        if (!teleportMode)
        {
            teleportMode = true;
            teleportCursorX = pc.x;
            teleportCursorY = pc.y;
        }
        else
        {

            char destTile = grid[teleportCursorY][teleportCursorX];
            if (hardness[teleportCursorY][teleportCursorX] != 255 && (destTile == '.' || destTile == '#' || destTile == '<' || destTile == '>'))
            {
                grid[pc.y][pc.x] = pc.prevTile;
                pc.place(teleportCursorX, teleportCursorY, destTile);
                grid[pc.y][pc.x] = '@'; /////
                teleportMode = false;
            }
            else
            {
                mvprintw(MAX_Y, 0, " Invalid teleport target! Pick floor/corridor/stairs.");
                refresh();
                getch(); // wait for keypress before continuing
            }
        }
        drawDungeon();
        return false;
    case 'r':
        if (teleportMode)
        {
            int x, y;
            do
            {
                x = rand() % MAX_X;
                y = rand() % MAX_Y;
            } while (hardness[y][x] == 255 || grid[y][x] == ' '); // can't go into immutable rock
            grid[pc.y][pc.x] = pc.prevTile; ///
            teleportCursorX = x;
            teleportCursorY = y;
            pc.place(x, y, grid[y][x]);
            teleportMode = false;
        }
        drawDungeon();
        return false;
    case 27: // ESC key
        teleportMode = false;
        drawDungeon();
        return false;

    case 'q':
        pc.place(-1, -1, pc.prevTile);
        return false;

    case 'm':
        showMonsterList();
        return false;

    case 'h':
        dx = -1;
        break;
    case 'l':
        dx = 1;
        break;
    case 'k':
        dy = -1;
        break;
    case 'j':
        dy = 1;
        break;
    case 'y':
        dx = -1;
        dy = -1;
        break;
    case 'u':
        dx = 1;
        dy = -1;
        break;
    case 'b':
        dx = -1;
        dy = 1;
        break;
    case 'n':
        dx = 1;
        dy = 1;
        break;
    case 'i':
    {
        clear();
        mvprintw(0, 0, "Inventory (slots 0-9):");

        for (int i = 0; i < 10; ++i)
        {
            Object *obj = pc.inventory[i];
            if (obj)
                mvprintw(i + 1, 0, "[%d] %s (%c)", i, obj->name.c_str(), obj->symbol);
            else
                mvprintw(i + 1, 0, "[%d] (empty)", i);
        }

        mvprintw(12, 0, "Press any key to return...");
        refresh();
        getch();
        return false;
    }
    case 'e':
    {
        clear();
        mvprintw(0, 0, "Equipped Items (slots a–l):");

        const char *slotNames[] = {
            "WEAPON", "OFFHAND", "RANGED", "ARMOR", "HELMET", "CLOAK",
            "GLOVES", "BOOTS", "AMULET", "LIGHT", "RING 1", "RING 2"};

        for (int i = 0; i < SLOT_COUNT; ++i)
        {
            Object *obj = pc.equipment[i];
            if (obj)
                mvprintw(i + 1, 0, "[%c] %s (%s)", 'a' + i, obj->name.c_str(), slotNames[i]);
            else
                mvprintw(i + 1, 0, "[%c] (empty) (%s)", 'a' + i, slotNames[i]);
        }

        mvprintw(SLOT_COUNT + 2, 0, "Press any key to return...");
        refresh();
        getch();
        return false;
    }
    case 'w':
    {
        clear();
        mvprintw(0, 0, "Wear item - Inventory (0–9), ESC to cancel:");
        for (int i = 0; i < 10; ++i)
        {
            if (pc.inventory[i])
                mvprintw(i + 1, 0, "[%d] %s (%s)", i, pc.inventory[i]->name.c_str(), objectTypeName(pc.inventory[i]->type).c_str());
        }

        refresh();
        int ch = getch();
        if (ch == 27)
            return false; // ESC = cancel

        int slot = ch - '0';
        if (slot < 0 || slot > 9 || !pc.inventory[slot])
            return false;

        Object *item = pc.inventory[slot];
        // int eqSlot = objectTypeToEquipSlot(item->type);
        int eqSlot = objectTypeToEquipSlot(item->type, pc);

        if (eqSlot == -1)
        {
            mvprintw(MAX_Y, 0, "Can't equip item of type %s", objectTypeName(item->type).c_str());
            refresh();
            getch();
            return false;
        }

        // Swap if needed
        std::swap(pc.equipment[eqSlot], pc.inventory[slot]);

        mvprintw(MAX_Y, 0, "Equipped %s to slot %c", item->name.c_str(), 'a' + eqSlot);
        refresh();
        getch();
        return false;
    }
    case 't':
    {
        clear();
        mvprintw(0, 0, "Take off item - Equipment (a-l), ESC to cancel:");

        const char *slotNames[] = {
            "WEAPON", "OFFHAND", "RANGED", "ARMOR", "HELMET", "CLOAK",
            "GLOVES", "BOOTS", "AMULET", "LIGHT", "RING 1", "RING 2"};

        for (int i = 0; i < SLOT_COUNT; ++i)
        {
            Object *obj = pc.equipment[i];
            if (obj)
                mvprintw(i + 1, 0, "[%c] %s (%s)", 'a' + i, obj->name.c_str(), slotNames[i]);
            else
                mvprintw(i + 1, 0, "[%c] (empty) (%s)", 'a' + i, slotNames[i]);
        }

        refresh();
        int ch = getch();
        if (ch == 27)
            return false; // ESC = cancel

        int eqSlot = ch - 'a';
        if (eqSlot < 0 || eqSlot >= SLOT_COUNT || !pc.equipment[eqSlot])
            return false;

        // Find free inventory slot
        for (int i = 0; i < 10; ++i)
        {
            if (!pc.inventory[i])
            {
                pc.inventory[i] = pc.equipment[eqSlot];
                pc.equipment[eqSlot] = nullptr;

                mvprintw(MAX_Y, 0, "Removed %s to inventory slot %d", pc.inventory[i]->name.c_str(), i);
                refresh();
                getch();
                return false;
            }
        }

        mvprintw(MAX_Y, 0, "Inventory full. Cannot unequip.");
        refresh();
        getch();
        return false;
    }
    case 'd':
    {
        clear();
        mvprintw(0, 0, "Drop item - Inventory (0-9), ESC to cancel:");

        for (int i = 0; i < 10; ++i)
        {
            Object *obj = pc.inventory[i];
            if (obj)
                mvprintw(i + 1, 0, "[%d] %s", i, obj->name.c_str());
            else
                mvprintw(i + 1, 0, "[%d] (empty)", i);
        }

        refresh();
        int ch = getch();
        if (ch == 27)
            return false; // ESC = cancel

        int invSlot = ch - '0';
        if (invSlot < 0 || invSlot >= 10 || !pc.inventory[invSlot])
            return false;

        Object *item = pc.inventory[invSlot];
        item->x = pc.x;
        item->y = pc.y;
        objects.push_back(item);

        pc.inventory[invSlot] = nullptr;

        mvprintw(MAX_Y, 0, "Dropped %s at (%d, %d)", item->name.c_str(), item->x, item->y);
        refresh();
        getch();
        return false;
    }
    case 'x': // Expunge
    {
        clear();
        mvprintw(0, 0, "Expunge item – Inventory (0–9), ESC to cancel:");
        for (int i = 0; i < 10; ++i)
        {
            if (pc.inventory[i])
            {
                mvprintw(i + 1, 0, "[%d] %s (%s)", i,
                         pc.inventory[i]->name.c_str(),
                         objectTypeName(pc.inventory[i]->type).c_str());
            }
            else
            {
                mvprintw(i + 1, 0, "[%d] (empty)", i);
            }
        }

        int key = getch();
        if (key == 27)
            break; // ESC to cancel
        int slot = key - '0';
        if (slot >= 0 && slot <= 9 && pc.inventory[slot])
        {
            delete pc.inventory[slot];
            pc.inventory[slot] = nullptr;
        }
        break;
    }
    case 'I':
    {
        clear();
        mvprintw(0, 0, "Inspect item – Inventory (0-9), ESC to cancel:");

        for (int i = 0; i < 10; ++i)
        {
            if (pc.inventory[i])
                mvprintw(i + 1, 0, "[%d] %s", i, pc.inventory[i]->name.c_str());
            else
                mvprintw(i + 1, 0, "[%d] (empty)", i);
        }

        refresh();
        int ch = getch();
        if (ch == 27)
            return false; // ESC to cancel

        int slot = ch - '0';
        if (slot < 0 || slot > 9 || !pc.inventory[slot])
            return false;

        Object *item = pc.inventory[slot];
        clear();
        mvprintw(0, 0, "Name: %s", item->name.c_str());
        mvprintw(1, 0, "Symbol: %c", item->symbol);
        mvprintw(2, 0, "Color: %s", item->color.c_str());
        mvprintw(3, 0, "Description:");
        mvprintw(4, 0, "%s", item->description.c_str());

        mvprintw(6, 0, "Press any key to continue...");
        refresh();
        getch();
        return false;
    }
    case 'L':
    {
        teleportMode = true;
        teleportCursorX = pc.x;
        teleportCursorY = pc.y;

        while (true)
        {
            drawDungeon();
            mvprintw(MAX_Y, 0, "Monster look mode – Move with hjkl, select with t, ESC to exit");
            move(teleportCursorY, teleportCursorX);
            refresh();

            int input = getch();

            if (input == 27)
            { // ESC
                teleportMode = false;
                drawDungeon();
                break;
            }

            int dx = 0, dy = 0;
            switch (input)
            {
            case 'h':
                dx = -1;
                break;
            case 'l':
                dx = 1;
                break;
            case 'k':
                dy = -1;
                break;
            case 'j':
                dy = 1;
                break;
            case 'y':
                dx = -1;
                dy = -1;
                break;
            case 'u':
                dx = 1;
                dy = -1;
                break;
            case 'b':
                dx = -1;
                dy = 1;
                break;
            case 'n':
                dx = 1;
                dy = 1;
                break;
            case 't':
            {
                bool inspected = false;
                for (const auto &obj : objects)
                {
                    if (obj->x == teleportCursorX && obj->y == teleportCursorY)
                    {
                        clear();
                        mvprintw(0, 0, "Item: %s", obj->name.c_str());
                        mvprintw(1, 0, "Symbol: %c", obj->symbol);
                        mvprintw(2, 0, "Color: %s", obj->color.c_str());
                        mvprintw(3, 0, "Description:");
                        mvprintw(4, 0, "%s", obj->description.c_str());
                        mvprintw(6, 0, "Press any key to return...");
                        refresh();
                        getch();
                        inspected = true;
                        break;
                    }
                }

                if (!inspected)
                {
                    for (const auto &m : monsters)
                    {
                        if (m.x == teleportCursorX && m.y == teleportCursorY)
                        {
                            clear();
                            mvprintw(0, 0, "Monster '%c'", m.type);
                            mvprintw(1, 0, "Speed: %d", m.speed);
                            mvprintw(2, 0, "HP: %d", m.hp);
                            mvprintw(3, 0, "Color: %s", m.color.c_str());
                            mvprintw(4, 0, "Abilities: I:%d T:%d E:%d D:%d", m.intelligence, m.telepathic, m.erratic, m.tunneling);
                            mvprintw(6, 0, "Press any key to return...");
                            refresh();
                            getch();
                            break;
                        }
                    }
                }
                break;
            }
            }

            teleportCursorX = std::clamp(teleportCursorX + dx, 0, MAX_X - 1);
            teleportCursorY = std::clamp(teleportCursorY + dy, 0, MAX_Y - 1);
        }

        return false;
    }

    default:
        return false; // invalid key = no turn taken
    }

    // If in teleport mode, just move the targeting cursor
    if (teleportMode)
    {
        teleportCursorX = std::clamp(teleportCursorX + dx, 0, MAX_X - 1);
        teleportCursorY = std::clamp(teleportCursorY + dy, 0, MAX_Y - 1);
        drawDungeon();
        return false; //  still no turn taken
    }

    int nx = pc.x + dx;
    int ny = pc.y + dy;
    for (Monster &m : monsters) /////////////////////////////////////
    {
        if (m.x == nx && m.y == ny)
        {
            attack(m);
            return true; // Count as a turn
        }
    }

    // Handle stairs
    if (grid[ny][nx] == '<' || grid[ny][nx] == '>')
    {
        regenerateDungeon();
        regenerateFlag = true;

        return false; //  Restarted, don't continue
    }

    // Normal move
    if (grid[ny][nx] == '.' || grid[ny][nx] == '#')
    {
        grid[pc.y][pc.x] = pc.prevTile;
        pc.place(nx, ny, grid[ny][nx]);
        grid[ny][nx] = '@';
        // Check for object pickup
        for (auto it = objects.begin(); it != objects.end(); ++it)
        {
            if ((*it)->x == pc.x && (*it)->y == pc.y)
            {
                // Find open inventory slot
                for (int i = 0; i < 10; ++i)
                {
                    if (!pc.inventory[i])
                    {
                        pc.inventory[i] = *it;
                        mvprintw(MAX_Y, 0, "Picked up: %s", (*it)->name.c_str());
                        refresh();
                        objects.erase(it);
                        return true; // done this turn
                    }
                }

                mvprintw(MAX_Y, 0, "Inventory full. Cannot pick up %s", (*it)->name.c_str());
                refresh();
                break;
            }
        }
    }

    return true; //  A turn was taken
}

void Dungeon::computeDistances(bool tunneling)
{
    int dist[MAX_Y][MAX_X];
    for (int y = 0; y < MAX_Y; y++)
        for (int x = 0; x < MAX_X; x++)
            dist[y][x] = INT_MAX;

    dist[pc.y][pc.x] = 0;

    using Node = std::tuple<int, int, int>;
    std::priority_queue<Node, std::vector<Node>, std::greater<>> pq;
    pq.emplace(0, pc.y, pc.x);

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    while (!pq.empty())
    {
        auto [d, y, x] = pq.top();
        pq.pop();

        for (int i = 0; i < 4; i++)
        {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (nx < 0 || ny < 0 || nx >= MAX_X || ny >= MAX_Y)
                continue;

            int cost = tunneling ? (1 + hardness[ny][nx] / 85) : 1;

            if (tunneling && hardness[ny][nx] == 255)
                continue;
            if (!tunneling && (grid[ny][nx] != '.' && grid[ny][nx] != '#'))
                continue;

            if (dist[ny][nx] > d + cost)
            {
                dist[ny][nx] = d + cost;
                pq.emplace(dist[ny][nx], ny, nx);
            }
        }
    }

    if (tunneling)
        memcpy(tunnelingDist, dist, sizeof(int) * MAX_Y * MAX_X);
    else
        memcpy(nonTunnelingDist, dist, sizeof(int) * MAX_Y * MAX_X);
}

std::pair<int, int> Dungeon::getNextStepTowardPC(int x, int y, bool tunneling)
{
    int bestX = x, bestY = y;
    int best = INT_MAX;

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    int (*dist)[MAX_X] = tunneling ? tunnelingDist : nonTunnelingDist;

    for (int i = 0; i < 4; i++)
    {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (nx < 0 || ny < 0 || nx >= MAX_X || ny >= MAX_Y)
            continue;

        if (dist[ny][nx] < best)
        {
            best = dist[ny][nx];
            bestX = nx;
            bestY = ny;
        }
    }

    return {bestX, bestY};
}

bool Dungeon::canSeePC(const Monster &m)
{
    return std::abs(m.x - pc.x) + std::abs(m.y - pc.y) <= 10;
}

void Dungeon::moveMonster(Monster &m)
{
    // Kill PC if adjacent
    // if (std::abs(m.x - pc.x) <= 1 && std::abs(m.y - pc.y) <= 1)
    // {
    //     grid[pc.y][pc.x] = m.type;
    //     pc.place(-1, -1, pc.prevTile);
    //     return;
    // }
    if (std::abs(m.x - pc.x) <= 1 && std::abs(m.y - pc.y) <= 1)
    {
        attackPC(m);
        return;
    }

    // Erratic monsters: 50% chance to move randomly
    if (m.erratic && rand() % 2 == 0)
    {
        int dirs[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
        for (int i = 0; i < 4; ++i)
        {
            int idx = rand() % 4;
            int nx = m.x + dirs[idx][0];
            int ny = m.y + dirs[idx][1];
            if (nx >= 0 && nx < MAX_X && ny >= 0 && ny < MAX_Y &&
                (grid[ny][nx] == '.' || grid[ny][nx] == '#'))
            {
                grid[m.y][m.x] = '.';
                m.place(nx, ny);
                grid[ny][nx] = m.type;
                return;
            }
        }
        return; // No valid random move found
    }

    // Intelligent + Telepathic
    if (m.intelligence && m.telepathic)
    {
        computeDistances(m.tunneling);
        auto [nx, ny] = getNextStepTowardPC(m.x, m.y, m.tunneling);
        if (grid[ny][nx] == '.' || grid[ny][nx] == '#')
        {
            grid[m.y][m.x] = '.';
            m.place(nx, ny);
            grid[ny][nx] = m.type;
        }
        else if (m.tunneling && hardness[ny][nx] > 0 && hardness[ny][nx] < 255)
        {
            hardness[ny][nx] -= 85;
            if (hardness[ny][nx] <= 0)
            {
                hardness[ny][nx] = 0;
                grid[ny][nx] = '#';
            }
        }
        return;
    }

    // Intelligent + Non-Telepathic → Use memory
    if (m.intelligence && !m.telepathic)
    {
        if (canSeePC(m))
        {
            m.lastSeenX = pc.x;
            m.lastSeenY = pc.y;
        }

        if (m.lastSeenX != -1 && m.lastSeenY != -1)
        {
            computeDistances(m.tunneling);

            // Temporarily treat last seen as target
            int oldPcX = pc.x, oldPcY = pc.y;
            pc.x = m.lastSeenX;
            pc.y = m.lastSeenY;
            auto [nx, ny] = getNextStepTowardPC(m.x, m.y, m.tunneling);
            pc.x = oldPcX;
            pc.y = oldPcY;

            if (grid[ny][nx] == '.' || grid[ny][nx] == '#')
            {
                grid[m.y][m.x] = '.';
                m.place(nx, ny);
                grid[ny][nx] = m.type;
            }
            else if (m.tunneling && hardness[ny][nx] > 0 && hardness[ny][nx] < 255)
            {
                hardness[ny][nx] -= 85;
                if (hardness[ny][nx] <= 0)
                {
                    hardness[ny][nx] = 0;
                    grid[ny][nx] = '#';
                }
            }
        }
        return;
    }

    // Default dumb chase
    int dx = (pc.x > m.x) - (pc.x < m.x);
    int dy = (pc.y > m.y) - (pc.y < m.y);
    int nx = m.x + dx;
    int ny = m.y + dy;

    if (grid[ny][nx] == '.' || grid[ny][nx] == '#')
    {
        grid[m.y][m.x] = '.';
        m.place(nx, ny);
        grid[ny][nx] = m.type;
    }
}
void Dungeon::showMonsterList()
{
    int highlight = 0;
    int ch;
    const int maxDisplay = LINES - 2;

    while (true)
    {
        clear();
        mvprintw(0, 0, "Monster List (w/s scroll, q to exit)");

        for (size_t i = highlight; i < monsters.size() && (int)(i - highlight) < maxDisplay; ++i)
        {
            const Monster &m = monsters[i];
            int dx = m.x - pc.x;
            int dy = m.y - pc.y;
            double dist = std::sqrt(dx * dx + dy * dy);

            std::string dir = "somewhere";
            if (dy < 0 && dx == 0)
                dir = "north";
            else if (dy > 0 && dx == 0)
                dir = "south";
            else if (dy == 0 && dx > 0)
                dir = "east";
            else if (dy == 0 && dx < 0)
                dir = "west";
            else if (dy < 0 && dx > 0)
                dir = "northeast";
            else if (dy < 0 && dx < 0)
                dir = "northwest";
            else if (dy > 0 && dx > 0)
                dir = "southeast";
            else if (dy > 0 && dx < 0)
                dir = "southwest";

            mvprintw(1 + (int)(i - highlight), 0,
                     "Monster '%c' is %.1f tiles to the %s [I:%d T:%d E:%d D:%d]",
                     m.type, dist, dir.c_str(),
                     m.intelligence, m.telepathic, m.erratic, m.tunneling);
        }

        refresh();
        ch = getch();
        if (ch == 'q')
            break;
        else if (ch == 's' && highlight + maxDisplay < (int)monsters.size())
            ++highlight;
        else if (ch == 'w' && highlight > 0)
            --highlight;
    }
}

const Player &Dungeon::getPlayer() const
{
    return pc;
}

void Dungeon::drawCorridor(int x1, int y1, int x2, int y2)
{
    while (x1 != x2 || y1 != y2)
    {
        if (rand() % 10 < 3) // 30% chance to zigzag (matches your original C code)
        {
            if (rand() % 2)
            {
                // Horizontal wiggle
                if (x1 > 1 && x1 < MAX_X - 2)
                {
                    x1 += (rand() % 2 == 0) ? 1 : -1;
                }
            }
            else
            {
                // Vertical wiggle
                if (y1 > 1 && y1 < MAX_Y - 2)
                {
                    y1 += (rand() % 2 == 0) ? 1 : -1;
                }
            }
        }
        else
        {
            if (x1 != x2)
                x1 += (x2 > x1) ? 1 : -1;
            else if (y1 != y2)
                y1 += (y2 > y1) ? 1 : -1;
        }

        // Only carve a corridor if it's currently an empty space to avoid destroying room walls
        if (grid[y1][x1] == ' ')
        {
            grid[y1][x1] = '#';
            hardness[y1][x1] = 0;
        }
    }
}

void Dungeon::regenerateDungeon()
{
    // Clear grid and hardness
    init();

    // Clear previous dungeon data
    rooms.clear();
    monsters.clear();
    clearObjects(); /////////////////////////////////////////////////////////////////

    // Re-generate everything in order
    generateRooms();
    connectRooms();
    placeStairs();
    placePlayer(); // PC placed
    // generateMonsters(MAX_MONSTERS);
    generateMonsters(numMonsters); // instead of MAX_MONSTERS

    computeDistances(false); // pathfinding
    computeDistances(true);

    // Also force cursor to PC position

    drawDungeon();
    // move(pc.y, pc.x);
    // refresh();
    mvprintw(MAX_Y, 0, "PC is at (%d, %d), tile: %c", pc.x, pc.y, grid[pc.y][pc.x]);
    refresh();
    getch();
}

// void Dungeon::generateObjects(int count)
// {
//     objects.clear();
//     for (int i = 0; i < count; ++i)
//     {
//         const ObjectDescription *chosen = nullptr;
//         for (int attempt = 0; attempt < 1000; ++attempt)
//         {
//             const ObjectDescription &candidate = objectTemplates[rand() % objectTemplates.size()];
//             if (rand() % 100 < candidate.rarity)
//             {
//                 chosen = &candidate;
//                 break;
//             }
//         }

//         if (!chosen)
//             continue;

//         Object *obj = chosen->generateInstance();

//         // Place in a random room
//         int attempts = 0;
//         while (attempts++ < 100)
//         {
//             Room r = rooms[rand() % rooms.size()];
//             int x = r.x + rand() % r.width;
//             int y = r.y + rand() % r.height;
//             if (grid[y][x] == '.' && !(x == pc.x && y == pc.y))
//             {
//                 obj->x = x;
//                 obj->y = y;
//                 objects.push_back(obj);
//                 break;
//             }
//         }
//     }
// }

void Dungeon::generateObjects(int count)
{
    objects.clear();

    for (int i = 0; i < count; ++i)
    {
        const ObjectDescription *chosen = nullptr;

        for (int attempt = 0; attempt < 1000; ++attempt)
        {
            const ObjectDescription &candidate = objectTemplates[rand() % objectTemplates.size()];

            // Step 1: Check rarity
            if (rand() % 100 >= candidate.rarity)
                continue;

            // Step 2: If it's an artifact and already generated, skip
            if (candidate.artifact && generatedArtifacts.count(candidate.name))
                continue;

            chosen = &candidate;
            break;
        }

        if (!chosen)
            continue;

        // Step 3: Mark artifact as generated
        if (chosen->artifact)
            generatedArtifacts.insert(chosen->name);

        // Step 4: Create the object instance
        Object *obj = chosen->generateInstance();

        // Step 5: Place it randomly
        int attempts = 0;
        while (attempts++ < 100)
        {
            Room r = rooms[rand() % rooms.size()];
            int x = r.x + rand() % r.width;
            int y = r.y + rand() % r.height;

            if (grid[y][x] == '.' && !(x == pc.x && y == pc.y))
            {
                obj->x = x;
                obj->y = y;
                objects.push_back(obj);
                break;
            }
        }
    }
}

void Dungeon::clearObjects()
{
    for (Object *obj : objects)
        delete obj;
    objects.clear();
}
