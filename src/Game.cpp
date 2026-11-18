#include "Game.h"

#include "Database.h"
#include "Extensions.h"
#include "Input.h"

#include <algorithm>
#include <iostream>

namespace
{
    // These match the SQL safety limits. You can experiment with game balance
    // in seed.sql or a mod, without changing the combat code.
    constexpr int maximumCurrency = 1000000;
    constexpr int maximumVictories = 1000000;
}

Game::Game(Database& database)
    : database_(database), enemies_(database.loadEnemies()), random_(std::random_device{}())
{
}

bool Game::chooseHero()
{
    const auto saved = database_.loadPlayer();
    if (saved)
    {
        std::cout << "\nSaved hero: " << saved->name << " (" << saved->victories << " victories)\n"
                  << "1. Continue\n2. New hero\n0. Quit\n";
        const auto choice = readChoice("> ", 0, 2);
        if (!choice || *choice == 0)
            return false;
        if (*choice == 1)
        {
            player_ = *saved;
            return true;
        }
        if (!readYesNo("Start fresh? Your old slot is replaced when you save. [y/n] ").value_or(false))
            return false;
    }
    else
    {
        std::cout << "\n1. Create a hero\n0. Quit\n";
        const auto choice = readChoice("> ", 0, 1);
        if (!choice || *choice == 0)
            return false;
    }

    const auto name = readHeroName();
    if (!name)
        return false;
    player_ = Player{}; // Reset every value to the defaults in Models.h.
    player_.name = *name;
    return true;
}

void Game::showStats() const
{
    // const means this function promises not to change the game state.
    std::cout << '\n' << player_.name << " | HP " << player_.health << '/' << player_.maxHealth
              << " | Attack " << player_.attack << "\nGold: " << player_.gold
              << " | Scrap: " << player_.scrap << " | Potions: " << player_.potions
              << " | Victories: " << player_.victories << '\n';
}

void Game::run()
{
    std::cout << "\n=== DUNGEON WORKSHOP ===\n"
              << "Explore the old tunnels. Bring scrap back to camp.\n"
              << "Four unfinished features are waiting in Extensions.cpp.\n";
    if (!chooseHero())
        return;

    while (true)
    {
        std::cout << "\n--- CAMP ---\n"
                  << "1. Explore a tunnel\n2. Rest (free full heal)\n3. Hero stats\n"
                  << "4. Workshop [TODO 2]\n5. Quest board [TODO 3]\n"
                  << "6. Use potion [TODO 1]\n7. Save & quit\n0. Quit without saving\n";
        const auto choice = readChoice("> ", 0, 7);
        if (!choice)
        {
            std::cout << "\nInput ended. No save written.\n";
            return;
        }

        switch (*choice)
        {
        case 1:
            if (!explore())
            {
                std::cout << "\nInput ended. No save written.\n";
                return;
            }
            break;
        case 2:
            player_.health = player_.maxHealth;
            std::cout << "You rest beside the campfire. Health fully restored.\n";
            break;
        case 3: showStats(); break;
        case 4: visitWorkshop(player_); break;
        case 5: visitQuestBoard(player_); break;
        case 6: usePotion(player_); break;
        case 7:
            database_.savePlayer(player_);
            std::cout << "Progress saved. See you back at camp!\n";
            return;
        case 0:
            if (readYesNo("Discard changes since your last save? [y/n] ").value_or(true))
                return;
            break;
        }
    }
}

bool Game::explore()
{
    if (enemies_.empty())
    {
        std::cout << "The tunnels are empty! Add an enemy using a SQL mod, then restart.\n";
        return true;
    }
    // vector stores a list. Distribution chooses a valid index, including both
    // endpoints. Loading enemies once keeps SQL out of the battle loop.
    std::uniform_int_distribution<std::size_t> pick(0, enemies_.size() - 1);
    return fight(enemies_[pick(random_)]);
}

bool Game::fight(Enemy enemy)
{
    std::cout << "\nA " << enemy.name << " blocks the tunnel!\n";
    while (enemy.health > 0 && player_.health > 0)
    {
        std::cout << "\nYour HP: " << player_.health << '/' << player_.maxHealth
                  << " | " << enemy.name << " HP: " << enemy.health << '\n'
                  << "1. Attack\n2. Potion [TODO 1]\n3. Special [TODO 4]\n0. Flee\n";
        const auto choice = readChoice("> ", 0, 3);
        if (!choice)
            return false;
        if (*choice == 0)
        {
            std::cout << "You escape to camp. No reward this time.\n";
            return true;
        }

        bool usedTurn = false;
        switch (*choice)
        {
        case 1:
        {
            const int damage = std::min(player_.attack, enemy.health);
            enemy.health -= damage;
            std::cout << "You deal " << damage << " damage.\n";
            usedTurn = true;
            break;
        }
        case 2: usedTurn = usePotion(player_); break;
        case 3: usedTurn = useSpecialAttack(player_, enemy); break;
        }

        // An unfinished/refused action costs no turn. A defeated enemy cannot
        // hit back. Try stepping through these conditions in the debugger.
        if (usedTurn && enemy.health > 0)
        {
            const int damage = std::min(enemy.attack, player_.health);
            player_.health -= damage;
            std::cout << enemy.name << " deals " << damage << " damage.\n";
        }
    }

    if (player_.health == 0)
    {
        const int lostGold = std::min(5, player_.gold);
        player_.gold -= lostGold;
        player_.health = player_.maxHealth;
        std::cout << "You wake up at camp, healed but missing " << lostGold << " gold.\n";
    }
    else
    {
        // Cap totals so they remain valid SQL values, even after many fights.
        const int earnedGold = std::min(enemy.goldReward, maximumCurrency - player_.gold);
        const int earnedScrap = std::min(enemy.scrapReward, maximumCurrency - player_.scrap);
        player_.gold += earnedGold;
        player_.scrap += earnedScrap;
        player_.victories = std::min(player_.victories + 1, maximumVictories);
        std::cout << "Victory! +" << earnedGold << " gold, +" << earnedScrap << " scrap.\n";
    }
    return true;
}
