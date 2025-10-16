#ifndef DICE_HPP
#define DICE_HPP

#include <string>
#include <random>
#include <sstream>

class Dice
{
public:
    int base;
    int dice;
    int sides;

    Dice(int base = 0, int dice = 0, int sides = 0)
        : base(base), dice(dice), sides(sides) {}

    int roll() const
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, sides);

        int result = base;
        for (int i = 0; i < dice; ++i)
            result += dis(gen);
        return result;
    }

    std::string toString() const
    {
        std::ostringstream oss;
        oss << base << "+" << dice << "d" << sides;
        return oss.str();
    }
};

#endif // DICE_HPP
