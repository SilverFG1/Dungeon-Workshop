#include "Input.h"

#include <iostream>
#include <sstream>

std::optional<int> readChoice(const std::string& prompt, int minimum, int maximum)
{
    while (true)
    {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line))
            return std::nullopt;

        // Reading the whole line avoids the leftover-newline problem caused
        // by mixing std::cin >> number with std::getline.
        std::istringstream input(line);
        int choice = 0;
        char extra = '\0';
        if ((input >> choice) && !(input >> extra)
            && choice >= minimum && choice <= maximum)
            return choice;

        std::cout << "Enter a whole number from " << minimum << " to " << maximum << ".\n";
    }
}

std::optional<bool> readYesNo(const std::string& prompt)
{
    while (true)
    {
        std::cout << prompt;
        std::string answer;
        if (!std::getline(std::cin, answer))
            return std::nullopt;
        if (answer == "y" || answer == "Y")
            return true;
        if (answer == "n" || answer == "N")
            return false;
        std::cout << "Enter y or n.\n";
    }
}

std::optional<std::string> readHeroName()
{
    while (true)
    {
        std::cout << "Hero name (1-30 characters): ";
        std::string name;
        if (!std::getline(std::cin, name))
            return std::nullopt;
        if (name.size() <= 30 && name.find_first_not_of(" \t\r") != std::string::npos)
            return name;
        std::cout << "Choose a name with 1-30 characters, including something besides spaces.\n";
    }
}
