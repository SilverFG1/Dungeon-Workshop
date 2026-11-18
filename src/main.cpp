#include "Database.h"
#include "Game.h"

#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    try
    {
        // CMake supplies this build folder, just as it supplies the SQL folder.
        // Even launching through PATH uses the same save. Rebuild the project
        // if you move it; --db lets you choose a different save location.
        auto databasePath = std::filesystem::path(GAME_BINARY_DIRECTORY) / "dungeon.db";
        std::vector<std::filesystem::path> mods;
        for (int index = 1; index < argc; ++index)
        {
            const std::string argument = argv[index];
            if (argument == "--help")
            {
                std::cout << "Dungeon Workshop\n"
                          << "  --db PATH     Use a different save/content database\n"
                          << "  --mod FILE    Apply a trusted SQL mod before playing (repeatable)\n"
                          << "Run without options to play. Read README.md to get started.\n";
                return 0;
            }
            if ((argument == "--db" || argument == "--mod") && index + 1 < argc)
            {
                const std::filesystem::path value = argv[++index];
                if (value.empty())
                    throw std::runtime_error("Option paths must not be empty.");
                if (argument == "--db")
                    databasePath = value;
                else
                    mods.push_back(value);
            }
            else
                throw std::runtime_error("Unknown option or missing value: " + argument + ". Try --help.");
        }

        Database database(databasePath.string());
        database.initialize(GAME_SQL_DIRECTORY); // CMake supplies this source path.
        for (const auto& mod : mods)
        {
            database.applyMod(mod);
            std::cout << "Applied SQL mod: " << mod.string() << '\n';
        }
        std::cout << "Database: " << std::filesystem::absolute(databasePath).string() << '\n';
        Game game(database);
        game.run();
    }
    catch (const std::exception& error)
    {
        // Database.cpp reports failures here instead of silently losing saves.
        std::cerr << "\nCould not continue: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
