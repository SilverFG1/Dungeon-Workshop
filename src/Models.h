#pragma once

#include <string>

// A struct groups related values together. Start here if C++ feels rusty.
// These are ordinary values, not database objects: C++ runs the game, while SQL
// stores a copy of the player and supplies the enemy definitions.
struct Player
{
    std::string name = "Rookie";
    int health = 30;
    int maxHealth = 30;
    int attack = 6;
    int gold = 0;
    int scrap = 0;
    int victories = 0;
    int potions = 1; // Saved already, ready for TODO 1 in Extensions.cpp.
};

struct Enemy
{
    int id = 0;
    std::string name;
    int health = 1;
    int attack = 1;
    int goldReward = 0;
    int scrapReward = 0;
};

// IDEA: add a defence value. Which structs, SQL columns, save statements and
// damage calculations would need to change? Change one part at a time.
