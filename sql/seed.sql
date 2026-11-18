-- These starting enemies are inserted ONCE, when a database is first created.
-- initialize() records that this happened in app_metadata in the same transaction.
-- Edit a mod script (or the database itself) to change an existing game's enemies.

INSERT INTO enemies (id, name, health, attack, gold_reward, scrap_reward) VALUES
    (1, 'Slime',       10, 2, 4, 1),
    (2, 'Cave Rat',    12, 3, 5, 1),
    (3, 'Rust Goblin', 16, 4, 7, 2);

-- TODO IDEA: design a fourth enemy, then add it in your own mod SQL file.
-- Start with small values so you can test a battle without losing immediately.
