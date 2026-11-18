-- A tiny gameplay mod: the three starting enemy types now deal 1 damage.
-- Applying this file again has the same effect (it is 'idempotent').
-- This changes the database permanently; to undo it, set attacks to 2, 3 and 4.
-- The game wraps this entire file in a transaction: do not add BEGIN or COMMIT.
-- Use only trusted local SQL scripts, since SQL can also remove stored data.

UPDATE enemies SET attack = 1 WHERE id IN (1, 2, 3);

-- TODO IDEA: copy this file as my_mod.sql and adjust enemy health or rewards.
-- TODO IDEA: add a Training Dummy with INSERT, choosing an unused positive id.
-- TODO IDEA: add a 'boss' column in a future schema migration and load it in C++.
