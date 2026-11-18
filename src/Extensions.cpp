#include "Extensions.h"

#include <iostream>

bool usePotion(Player& player)
{
    std::cout << "You have " << player.potions << " potion(s).\n";
    std::cout << "Potion use is not implemented yet. Try TODO 1!\n";

	if (player.usePotion()) 
    {
		std::cout << "You drink a potion and restore some health.\n";
		player.health = std::min(player.maxHealth, player.health + 10);
        player.potions--;
        std::cout << "Your health is now " << player.health << '/' << player.maxHealth << ".\n";
        return true;
	}
	else 
    {
		std::cout << "You cannot use a potion right now.\n";
	}

    // TODO 1 (small): make a potion restore 10 health.
    // 1. If there are no potions OR health is already full, return false.
    // 2. Set health to std::min(player.maxHealth, player.health + 10).
    //    std::min lives in <algorithm>, so add that include above.
    // 3. Subtract one potion, print the new health, and return true.
    // Player& is a reference: changing it changes the actual hero in Game.cpp.
    // CHECK: 27/30 health becomes 30/30, and one potion becomes zero.
    return false; // A placeholder must not consume a battle turn.
}

void visitWorkshop(Player& player)
{
    const int scrapCost = 3;
    std::cout << "\n--- THE WORKSHOP (unfinished) ---\n"
              << "Scrap: " << player.scrap << " | Attack: " << player.attack << '\n'
              << "Planned recipe: " << scrapCost << " scrap -> +1 attack.\n"
              << "Upgrades are not implemented yet. Try TODO 2!\n";

	if player.scrap >= scrapCost
	{
		std::cout << "You have enough scrap to upgrade your attack.\n";
	    player.attack = std::min(10000, player.attack + 1); // Ensure attack does not exceed the SQL limit.
	    player.scrap -= scrapCost;
	}
	else
	{
		std::cout << "You need " << scrapCost << " scrap to upgrade your attack.\n";
	}

    // TODO 2 (small): offer this recipe and ask the player to confirm it.
    // Reuse readYesNo from Input.h. Check scrap >= scrapCost BEFORE subtracting.
    // Increase attack only after payment; keep it <= 10000 (the SQL limit).
    // No SQL is needed here: Save & quit already stores both of these values.
    // CHECK: 2 scrap cannot buy it; 3 scrap buys it once and leaves zero.
    // IDEA: later, load a recipes table from SQLite instead of hardcoding prices.
}

void visitQuestBoard(const Player& player)
{
    const int target = 3;
    std::cout << "\n--- QUEST BOARD (unfinished) ---\n"
        << "Clear the tunnels: defeat " << target << " enemies.\n"
        << "Progress: " << player.victories << '/' << target << " | Planned reward: 10 gold.\n"
        << "Claiming rewards is not implemented yet. Try TODO 3!\n";

    if player.victories >= target
    {
        std::cout << "You have completed the quest! You can claim your reward.\n";
        player.gold += 10;
        player.questCompleted = true;
        // once true is set, it should remain true for the rest of the game session and remove text option to claim reward again.
        player.victories = 0; // Reset victories after claiming the reward... this is a temporary solution until a more robust quest tracking system is implemented.
    }
    else
    {
        std::cout << "You have not yet completed the quest. Keep exploring!\n";
        std::cout << "You need to defeat " << (target - player.victories) << " more enemies to complete the quest.\n";
    }

    // TODO 3 (medium): let a player claim this reward exactly once.
    // Start with victories >= target. Then work out how to remember a claim:
    // add a bool to Player AND a column in player_save; update save/load too.
    // You will also need Player& here (remove const) to change the hero.
    // Existing databases need ALTER TABLE, or use a NEW --db file to practise.
    // CHECK: claiming twice, or saving and reloading, must not give extra gold.
    // IDEA: make a quests table and join it to a player_quests progress table.
}

bool useSpecialAttack(Player& player, Enemy& enemy)
{
    std::cout << player.name << " prepares a special attack against " << enemy.name << ".\n"
              << "Special attacks are not implemented yet. Try TODO 4!\n";

    // TODO 4 (medium): deal twice player.attack, once per encounter.
    // First add a specialUsed bool INSIDE Game::fight so each fight resets it.
    // Then pass that flag here by reference and update this declaration in .h.
    // Clamp enemy health to zero, print the damage, and return true on success.
    // CHECK: the second attempt in one fight is refused without losing a turn;
    // the special becomes available again in the next fight.
    return false;
}
