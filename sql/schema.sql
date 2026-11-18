-- CREATE TABLE IF NOT EXISTS makes startup safe to repeat.
-- Later, changing this file will NOT change an existing table automatically.
-- TODO IDEA: learn schema migrations once you are ready to add more columns.

CREATE TABLE IF NOT EXISTS app_metadata (
    key TEXT PRIMARY KEY NOT NULL,
    value TEXT NOT NULL
);

-- One row describes an enemy type. The C++ game loads these rows into structs.
CREATE TABLE IF NOT EXISTS enemies (
    id INTEGER PRIMARY KEY CHECK (id BETWEEN 1 AND 1000000),
    name TEXT NOT NULL CHECK (length(trim(name)) BETWEEN 1 AND 60),
    health INTEGER NOT NULL CHECK (health BETWEEN 1 AND 10000),
    attack INTEGER NOT NULL CHECK (attack BETWEEN 1 AND 10000),
    gold_reward INTEGER NOT NULL CHECK (gold_reward BETWEEN 0 AND 1000000),
    scrap_reward INTEGER NOT NULL CHECK (scrap_reward BETWEEN 0 AND 1000000)
);

-- For now there is one save slot. A CHECK constraint rejects a second slot.
-- CHECK constraints also reject accidental negative or excessively large stats.
CREATE TABLE IF NOT EXISTS player_save (
    id INTEGER PRIMARY KEY CHECK (id = 1),
    name TEXT NOT NULL CHECK (length(trim(name)) BETWEEN 1 AND 60),
    health INTEGER NOT NULL CHECK (health BETWEEN 0 AND 10000),
    max_health INTEGER NOT NULL CHECK (max_health BETWEEN 1 AND 10000),
    attack INTEGER NOT NULL CHECK (attack BETWEEN 1 AND 10000),
    gold INTEGER NOT NULL CHECK (gold BETWEEN 0 AND 1000000),
    scrap INTEGER NOT NULL CHECK (scrap BETWEEN 0 AND 1000000),
    victories INTEGER NOT NULL CHECK (victories BETWEEN 0 AND 1000000),
    potions INTEGER NOT NULL CHECK (potions BETWEEN 0 AND 10000),
    CHECK (health <= max_health)
);
