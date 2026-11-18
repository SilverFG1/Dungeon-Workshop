#pragma once

#include "Models.h"

#include <random>
#include <vector>

class Database;

class Game
{
public:
    explicit Game(Database& database);
    void run();

private:
    // A class groups state and the functions that work on that state.
    Database& database_;
    Player player_;
    std::vector<Enemy> enemies_;
    std::mt19937 random_; // Seeded once, rather than once per random choice.

    bool chooseHero();
    void showStats() const;
    bool explore();
    bool fight(Enemy enemy); // A copy: damage must not edit the SQL template.
};
