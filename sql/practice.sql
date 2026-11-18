-- Open the game's .db file in a SQLite editor, then try these read-only queries.
-- Running this whole file is safe: the exercises below are comments.

-- 1. Inspect the complete enemy table, in a predictable order.
SELECT * FROM enemies ORDER BY id;

-- 2. Select only the columns you want and filter rows with WHERE.
SELECT name, health, attack
FROM enemies
WHERE attack <= 3
ORDER BY health;

-- 3. Calculate a new value without changing stored data.
SELECT name, gold_reward + scrap_reward AS total_reward
FROM enemies
ORDER BY total_reward DESC;

-- 4. An aggregate turns many enemy rows into one result.
SELECT COUNT(*) AS enemy_types, AVG(health) AS average_health
FROM enemies;

-- 5. This returns no rows until you save a player from the game.
SELECT name, health, max_health, gold, scrap, victories
FROM player_save;

-- EXERCISE: write a SELECT that finds the enemy with the highest health.
-- Hint: combine ORDER BY health DESC with LIMIT 1.
--
-- EXERCISE: select enemies where health is at least 12 AND gold_reward is above 4.
--
-- EXERCISE: show each enemy's gold per point of health.
-- Hint: use 1.0 * gold_reward / health to get decimal division.
--
-- NEXT STEP: copy sql/mods/gentle_mode.sql and make your own balance changes.
-- Learn UPDATE and INSERT there; keep this file as a read-only practice space.
