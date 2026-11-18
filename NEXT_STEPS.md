# Your next coding sessions

Work through one TODO at a time. Build after each small change, then play the relevant menu option. A feature that still prints a placeholder message is deliberately unfinished.

Most exercises start in `src/Extensions.cpp`. The comments there are hints; you decide how to turn them into working C++.

## TODO 1: Make potions work

**Practise:** `if` statements, changing a struct, and limiting a number.

Use one potion to restore health. Check that the hero has a potion first, keep health at or below `maxHealth`, and subtract one potion only when it is used. Give the player a useful message for each result.

Start with the potion count and health fields in `src/Models.h`. You do not need to change SQL for this exercise: those values already belong to the hero.

Check your result:

- A wounded hero gains health and loses exactly one potion.
- Healing near maximum health never exceeds `maxHealth`.
- Zero potions leaves health and stock unchanged. Decide whether a full-health hero should keep their potion, then make the message match that behaviour.

Try it from both the camp menu and the battle menu. Read the surrounding battle code to understand when an enemy gets its turn.

## TODO 2: Add a workshop upgrade

**Practise:** a resource check, arithmetic, and a maximum value.

Make the workshop trade **3 scrap for +1 attack**. Explain the cost and the result in the menu message. Keep attack at or below **10,000**.

Think about the order of your checks: a player should spend scrap only when the upgrade can succeed. Use the existing `scrap` and `attack` fields; no new inventory system is needed.

Check your result:

- With 2 scrap, the hero gets no upgrade and spends nothing.
- With 3 scrap, the hero gains 1 attack and has 0 scrap left.
- At the attack limit, neither attack nor scrap changes.

Save and restart after a successful upgrade. Check the stats screen to see that both changed values were saved.

## TODO 3: Finish the quest board

**Practise:** conditions, state that survives a restart, and a small database change.

Reward the hero with **10 gold after 3 victories**. Show progress before the target is reached. Once the reward is claimed, show that the quest is complete.

The existing `victories` field supplies the progress. You also need a claimed flag so opening the board repeatedly cannot award more gold. Add that state to the hero model, then follow the existing save/load fields through `src/Database.cpp` and `sql/schema.sql` so the flag survives restarting the game.

Use a fresh `--db` filename after changing the schema. Editing `schema.sql` does not update an existing database. Schema migrations can be a later project.

Check your result:

- At fewer than 3 victories, there is no reward.
- At 3 or more victories, claiming adds exactly 10 gold once.
- Reopening the board gives no extra gold. Saving, restarting, and reopening it also gives no extra gold.

This is a larger exercise than the first two. Trace one existing field from the model into the database and back before adding your own.

## TODO 4: Add a special attack

**Practise:** function parameters and state with a limited lifetime.

Make the special attack use **twice the hero's normal attack power**, available **once per fight**. Follow the normal attack's damage handling so the result stays consistent with the rest of combat.

The flag recording whether the special was used belongs inside `Game::fight`. Each new call starts a new fight, so it should start with the special available again. Pass information to the extension function as needed; avoid making this a global variable or a saved hero field.

Check your result:

- The first special attack uses twice the normal attack power.
- A second attempt in the same fight cannot deal special damage again.
- After fleeing or finishing the fight, a new encounter allows the special again.

Watch the enemy-turn logic as well: decide what should happen after an unavailable action and make the message and turn behaviour agree.

## A small SQL session

Read `sql/practice.sql` and use the [SQLite language reference](https://www.sqlite.org/lang.html) when a keyword is unfamiliar. If you use a SQLite editor, open an experimental database rather than your main save.

Try these in order:

1. Read all enemies, then select only the columns you want to inspect.
2. Filter the results to weaker enemies and sort them by a combat stat.
3. Read `sql/mods/gentle_mode.sql` and explain what its `UPDATE` changes.
4. Add an enemy to `sql/seed.sql`, using an existing row as a guide. Start a new database and see whether you can meet it.

Keep an eye on the difference between C++ and SQL: changing a combat calculation needs a rebuild; changing an existing database needs an SQL statement. Changing seed data affects the next freshly created database.

## Keep the next change small

Once those features work, pick one idea: a second quest, a new enemy, or a different workshop upgrade. Write down its cost, effect, and one way you will check it before coding. You can build a bigger game later; each working feature gives you a solid place to continue.
