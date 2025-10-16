#ifndef EVENT_HPP
#define EVENT_HPP

#include "Monster.hpp"
#include "Player.hpp"
#include <queue>

struct Event
{
    int time;
    bool isPlayer;
    Monster *monster;

    Event(int t, Monster *m = nullptr, bool isPc = false)
        : time(t), isPlayer(isPc), monster(m) {}

    // Min-heap based on time
    bool operator>(const Event &other) const
    {
        return time > other.time;
    }
};

class EventQueue
{
public:
    void push(const Event &e);
    Event pop();
    bool empty() const;

private:
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> queue;
};

#endif
