#include "Database.h"

// Windows includes SQLite with the operating system. Other platforms use the
// normal SQLite development package; the database API is the same on both.
#ifdef _WIN32
#include <winsqlite/winsqlite3.h>
#else
#include <sqlite3.h>
#endif

#include <fstream>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace {

// A prepared statement is a resource, like an open file. unique_ptr runs this
// cleanup even when an exception leaves a function early (this is called RAII).
struct StatementCloser {
    void operator()(sqlite3_stmt* statement) const {
        sqlite3_finalize(statement);
    }
};
using Statement = std::unique_ptr<sqlite3_stmt, StatementCloser>;

Statement prepare(sqlite3* connection, const char* sql) {
    sqlite3_stmt* rawStatement = nullptr;
    const int result = sqlite3_prepare_v2(connection, sql, -1, &rawStatement, nullptr);
    Statement statement(rawStatement);
    if (result != SQLITE_OK) {
        throw std::runtime_error("Could not prepare SQL: " +
                                 std::string(sqlite3_errmsg(connection)));
    }
    return statement;
}

void checkBind(sqlite3* connection, int result) {
    if (result != SQLITE_OK) {
        throw std::runtime_error("Could not bind a saved value: " +
                                 std::string(sqlite3_errmsg(connection)));
    }
}

std::string readSqlFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open SQL file: " + path.string());
    }
    std::string sql((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
    if (file.bad()) {
        throw std::runtime_error("Could not read SQL file: " + path.string());
    }
    return sql;
}

// Match schema.sql's limits before sending a save to SQLite. Keeping both checks
// gives a useful C++ error while SQL still protects edits made outside the game.
void validatePlayer(const Player& player) {
    if (player.name.empty() || player.name.size() > 60 ||
        player.name.find('\0') != std::string::npos) {
        throw std::runtime_error("A saved player needs a name of 1 to 60 bytes.");
    }
    if (player.maxHealth < 1 || player.maxHealth > 10000 ||
        player.health < 0 || player.health > player.maxHealth ||
        player.attack < 1 || player.attack > 10000 ||
        player.gold < 0 || player.gold > 1000000 ||
        player.scrap < 0 || player.scrap > 1000000 ||
        player.victories < 0 || player.victories > 1000000 ||
        player.potions < 0 || player.potions > 10000) {
        throw std::runtime_error("Player stats are outside the limits in schema.sql.");
    }
}

} // namespace

Database::Database(const std::string& path) {
    const int result = sqlite3_open_v2(path.c_str(), &connection_,
                                     SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                                     nullptr);
    if (result != SQLITE_OK) {
        const std::string message = connection_ ? sqlite3_errmsg(connection_)
                                               : "not enough memory";
        sqlite3_close(connection_);
        connection_ = nullptr;
        throw std::runtime_error("Could not open database '" + path + "': " + message);
    }
    // Give another program (such as a database editor) a short time to finish
    // writing before reporting that the database is busy.
    sqlite3_busy_timeout(connection_, 3000);
}

Database::~Database() {
    sqlite3_close(connection_);
}

void Database::execute(const std::string& sql) {
    char* errorMessage = nullptr;
    const int result = sqlite3_exec(connection_, sql.c_str(), nullptr, nullptr,
                                    &errorMessage);
    if (result != SQLITE_OK) {
        const std::string message = errorMessage ? errorMessage
                                                : sqlite3_errmsg(connection_);
        sqlite3_free(errorMessage);
        throw std::runtime_error("SQL failed: " + message);
    }
}

void Database::rollback() noexcept {
    // Preserve the original exception if rolling back also fails.
    sqlite3_exec(connection_, "ROLLBACK;", nullptr, nullptr, nullptr);
}

void Database::initialize(const std::filesystem::path& sqlDirectory) {
    const std::string schema = readSqlFile(sqlDirectory / "schema.sql");
    execute("BEGIN IMMEDIATE;");
    try {
        execute(schema);
        bool alreadySeeded = false;
        {
            auto statement = prepare(connection_,
                "SELECT 1 FROM app_metadata WHERE key = 'initial_enemy_seed';");
            const int result = sqlite3_step(statement.get());
            if (result != SQLITE_ROW && result != SQLITE_DONE) {
                throw std::runtime_error("Could not check initial data: " +
                                         std::string(sqlite3_errmsg(connection_)));
            }
            alreadySeeded = result == SQLITE_ROW;
        }
        // Seed ONCE per database, so a mod's deleted enemies stay deleted.
        // Editing seed.sql later changes new databases only. Change an existing
        // database with a mod or with your SQLite editor instead.
        if (!alreadySeeded) {
            execute(readSqlFile(sqlDirectory / "seed.sql"));
            execute("INSERT INTO app_metadata (key, value) "
                    "VALUES ('initial_enemy_seed', '1');");
        }
        execute("COMMIT;");
    } catch (...) {
        rollback();
        throw;
    }
}

std::vector<Enemy> Database::loadEnemies() {
    auto statement = prepare(connection_,
        "SELECT id, name, health, attack, gold_reward, scrap_reward "
        "FROM enemies ORDER BY id;");
    std::vector<Enemy> enemies;
    int result = SQLITE_OK;
    while ((result = sqlite3_step(statement.get())) == SQLITE_ROW) {
        Enemy enemy;
        enemy.id = sqlite3_column_int(statement.get(), 0);
        enemy.name = reinterpret_cast<const char*>(sqlite3_column_text(statement.get(), 1));
        enemy.health = sqlite3_column_int(statement.get(), 2);
        enemy.attack = sqlite3_column_int(statement.get(), 3);
        enemy.goldReward = sqlite3_column_int(statement.get(), 4);
        enemy.scrapReward = sqlite3_column_int(statement.get(), 5);
        enemies.push_back(enemy);
    }
    if (result != SQLITE_DONE) {
        throw std::runtime_error("Could not load enemies: " +
                                 std::string(sqlite3_errmsg(connection_)));
    }
    // TODO IDEA: add a 'region' column, then load only one dungeon's enemies.
    return enemies;
}

std::optional<Player> Database::loadPlayer() {
    auto statement = prepare(connection_,
        "SELECT name, health, max_health, attack, gold, scrap, victories, potions "
        "FROM player_save WHERE id = 1;");
    const int result = sqlite3_step(statement.get());
    if (result == SQLITE_DONE) {
        return std::nullopt; // No save yet: the game can create a new player.
    }
    if (result != SQLITE_ROW) {
        throw std::runtime_error("Could not load player: " +
                                 std::string(sqlite3_errmsg(connection_)));
    }
    Player player;
    player.name = reinterpret_cast<const char*>(sqlite3_column_text(statement.get(), 0));
    player.health = sqlite3_column_int(statement.get(), 1);
    player.maxHealth = sqlite3_column_int(statement.get(), 2);
    player.attack = sqlite3_column_int(statement.get(), 3);
    player.gold = sqlite3_column_int(statement.get(), 4);
    player.scrap = sqlite3_column_int(statement.get(), 5);
    player.victories = sqlite3_column_int(statement.get(), 6);
    player.potions = sqlite3_column_int(statement.get(), 7);
    validatePlayer(player);
    return player;
}

void Database::savePlayer(const Player& player) {
    validatePlayer(player);
    // '?' placeholders keep values separate from SQL. A name such as O'Brien
    // works correctly, without having to escape quotation marks ourselves.
    auto statement = prepare(connection_,
        "INSERT OR REPLACE INTO player_save "
        "(id, name, health, max_health, attack, gold, scrap, victories, potions) "
        "VALUES (1, ?, ?, ?, ?, ?, ?, ?, ?);");
    checkBind(connection_, sqlite3_bind_text(statement.get(), 1, player.name.c_str(),
              static_cast<int>(player.name.size()), SQLITE_TRANSIENT));
    const int values[] = {player.health, player.maxHealth, player.attack, player.gold,
                          player.scrap, player.victories, player.potions};
    for (int index = 0; index < 7; ++index) {
        checkBind(connection_, sqlite3_bind_int(statement.get(), index + 2, values[index]));
    }
    if (sqlite3_step(statement.get()) != SQLITE_DONE) {
        throw std::runtime_error("Could not save player: " +
                                 std::string(sqlite3_errmsg(connection_)));
    }
    // TODO IDEA: replace the fixed slot '1' with a parameter to add save slots.
}

void Database::applyMod(const std::filesystem::path& file) {
    // SQL files can change or delete data. Apply only your own trusted local
    // scripts. Leave BEGIN/COMMIT out of them; this function owns the transaction.
    const std::string sql = readSqlFile(file);
    execute("BEGIN IMMEDIATE;");
    try {
        execute(sql);
        execute("COMMIT;");
    } catch (...) {
        rollback();
        throw;
    }
}
