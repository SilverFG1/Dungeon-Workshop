#include "Database.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <typename Action>
void expectFailure(Action action, const std::string& message) {
    try {
        action();
    } catch (const std::exception&) {
        return;
    }
    throw std::runtime_error(message);
}

// Tests use a real file so closing and reopening exercises disk persistence.
// This object owns only the directory it successfully creates.
struct TemporaryDirectory {
    // CTest runs in the build directory, which is writable in this workspace.
    std::filesystem::path parent = std::filesystem::current_path();
    std::filesystem::path path;

    TemporaryDirectory() {
        const auto tick = std::chrono::steady_clock::now().time_since_epoch().count();
        path = std::filesystem::absolute(parent / ("dungeon-workshop-test-" +
            std::to_string(tick) + "-" + std::to_string(std::random_device{}())))
            .lexically_normal();
        parent = std::filesystem::absolute(parent).lexically_normal();
        expect(std::filesystem::create_directory(path), "Could not create test directory.");
    }

    ~TemporaryDirectory() {
        // Check the exact absolute target before recursive cleanup. Never delete
        // the working directory or a directory that this object did not create.
        if (path.parent_path() == parent &&
            path.filename().string().find("dungeon-workshop-test-") == 0) {
            std::error_code ignored;
            std::filesystem::remove_all(path, ignored);
        }
    }
};

void writeSql(const std::filesystem::path& path, const std::string& sql) {
    std::ofstream file(path);
    file << sql;
    expect(static_cast<bool>(file), "Could not write a temporary mod.");
}

void expectPlayer(const std::optional<Player>& loaded, const Player& expected) {
    expect(loaded.has_value(), "Expected a saved player.");
    const Player& actual = *loaded;
    expect(actual.name == expected.name && actual.health == expected.health &&
           actual.maxHealth == expected.maxHealth && actual.attack == expected.attack &&
           actual.gold == expected.gold && actual.scrap == expected.scrap &&
           actual.victories == expected.victories && actual.potions == expected.potions,
           "Every player field should survive a save and load.");
}

void expectGentleEnemies(const std::vector<Enemy>& enemies, std::size_t count) {
    expect(enemies.size() == count, "Unexpected enemy count after applying a mod.");
    for (const Enemy& enemy : enemies) {
        expect(enemy.attack == 1, "Gentle mode should keep enemy attacks at 1.");
    }
}

} // namespace

int main() {
    try {
        TemporaryDirectory temporary;
        const auto databasePath = temporary.path / "test.db";
        const auto sqlDirectory = std::filesystem::path(GAME_SQL_DIRECTORY);
        const auto deleteMod = temporary.path / "delete_enemy.sql";
        const auto brokenMod = temporary.path / "broken_mod.sql";
        writeSql(deleteMod, "DELETE FROM enemies WHERE id = 2;");
        writeSql(brokenMod,
            "UPDATE enemies SET health = 999 WHERE id = 1;\n"
            "THIS IS DELIBERATELY INVALID SQL;\n");

        Player expected;
        expected.name = "O'Brien"; // Parameters must handle apostrophes correctly.
        expected.health = 23;
        expected.maxHealth = 40;
        expected.attack = 9;
        expected.gold = 41;
        expected.scrap = 7;
        expected.victories = 6;
        expected.potions = 3;

        {
            Database database(databasePath.string());
            database.initialize(sqlDirectory);
            const auto enemies = database.loadEnemies();
            expect(enemies.size() == 3, "A new database needs three starting enemies.");
            expect(enemies[0].name == "Slime" && enemies[0].health == 10 &&
                   enemies[0].attack == 2 && enemies[0].goldReward == 4 &&
                   enemies[0].scrapReward == 1,
                   "The first starting enemy should match seed.sql.");
            expect(!database.loadPlayer(), "A new database should have no saved player.");
            database.savePlayer(expected);
            expectPlayer(database.loadPlayer(), expected);
            database.applyMod(sqlDirectory / "mods" / "gentle_mode.sql");
            database.applyMod(sqlDirectory / "mods" / "gentle_mode.sql");
            expectGentleEnemies(database.loadEnemies(), 3);
        } // The connection really closes here, before the next Database opens.

        {
            Database database(databasePath.string());
            database.initialize(sqlDirectory);
            expectPlayer(database.loadPlayer(), expected);
            expectGentleEnemies(database.loadEnemies(), 3);

            database.applyMod(deleteMod);
            database.initialize(sqlDirectory);
            const auto enemies = database.loadEnemies();
            expect(enemies.size() == 2 && enemies[0].id == 1 && enemies[1].id == 3,
                   "Repeated initialization must not restore a deleted enemy.");
            const int previousHealth = enemies[0].health;
            expectFailure([&] { database.applyMod(brokenMod); },
                          "An invalid SQL mod should report an error.");
            expect(database.loadEnemies()[0].health == previousHealth,
                   "A failed mod must roll back its earlier successful UPDATE.");

            Player invalid = expected;
            invalid.health = invalid.maxHealth + 1;
            expectFailure([&] { database.savePlayer(invalid); },
                          "An invalid player save should report an error.");
            expectPlayer(database.loadPlayer(), expected);
            database.initialize(sqlDirectory);
            expectGentleEnemies(database.loadEnemies(), 2);
            expectPlayer(database.loadPlayer(), expected);
        }

        {
            Database database(databasePath.string());
            database.initialize(sqlDirectory);
            expectGentleEnemies(database.loadEnemies(), 2);
            expectPlayer(database.loadPlayer(), expected);
        }

        std::cout << "Database smoke checks passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Database smoke check failed: " << error.what() << '\n';
        return 1;
    }
}
