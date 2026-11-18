#pragma once

#include <optional>
#include <string>

// optional means "there might not be an answer". An empty result means input
// ended (Ctrl+Z then Enter on Windows), so the game can stop without spinning.
std::optional<int> readChoice(const std::string& prompt, int minimum, int maximum);
std::optional<bool> readYesNo(const std::string& prompt);
std::optional<std::string> readHeroName();
