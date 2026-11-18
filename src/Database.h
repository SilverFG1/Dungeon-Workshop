#pragma once

#include "Models.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

// A forward declaration keeps SQLite's platform-specific header out of the rest
// of the game. Only Database.cpp needs to know how SQLite works internally.
struct sqlite3;

class Database {
public:
    explicit Database(const std::string& path);
    ~Database();

    // Two objects must never both own (and close) the same connection.
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    void initialize(const std::filesystem::path& sqlDirectory);
    std::vector<Enemy> loadEnemies();
    std::optional<Player> loadPlayer();
    void savePlayer(const Player& player);
    void applyMod(const std::filesystem::path& file);

private:
    sqlite3* connection_ = nullptr;

    void execute(const std::string& sql);
    void rollback() noexcept;
};
