#pragma once

#include "Models.h"

// These sections are deliberately unfinished, but the whole project compiles.
// A bool return tells combat whether an action actually used the player's turn.
bool usePotion(Player& player);
void visitWorkshop(Player& player);
void visitQuestBoard(const Player& player);
bool useSpecialAttack(Player& player, Enemy& enemy);
