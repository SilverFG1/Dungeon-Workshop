# Dungeon Workshop

A small C++17 console game you can finish and change as you get back into coding. Explore a dungeon, fight enemies, collect gold and scrap, and save your hero. SQLite supplies the enemy data and stores one saved hero.

The basic game works. Four features deliberately stop at a helpful placeholder, with `TODO` comments pointing you towards the next step. No graphics, engine, or large framework is involved.

## Start on Windows

Install **Desktop development with C++** through the Visual Studio Installer, including a Windows SDK and the C++ CMake tools. The project uses the Windows SDK's built-in `winsqlite3` library, so you do not need to download SQLite separately.

Open this project folder and double-click **Play.cmd**, or run:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\play.ps1
```

The launcher finds CMake, builds the Debug version, and starts the game. You can keep using any editor you like for the source files.

To build manually, run these commands from this folder:

```powershell
cmake -S . -B build
cmake --build build --config Debug
.\build\Debug\dungeon_workshop.exe
```

If your CMake generator uses a single build configuration, the executable is instead at `.\build\dungeon_workshop.exe`.

## Linux and macOS

Install a C++17 compiler, CMake, and the SQLite development library for your system. CMake uses `find_package(SQLite3)` on these platforms.

```sh
cmake -S . -B build
cmake --build build --config Debug
./build/dungeon_workshop
```

A generator that separates configurations may place the executable inside `build/Debug`.

## What you can play

| Section | Current state |
| --- | --- |
| Explore and fight | Working: normal attacks, enemy turns, victory rewards |
| Flee | Working: guaranteed escape, with no reward |
| Defeat | Working: return to camp healed and lose up to 5 gold |
| Rest and stats | Working: free healing at camp and a view of your hero |
| Save and load | Working: one saved hero in the selected database |
| Potions | Started: finish healing and stock handling in TODO 1 |
| Workshop | Started: spend scrap on an attack upgrade in TODO 2 |
| Quest board | Started: finish a victory reward in TODO 3 |
| Special attack | Started: add a once-per-fight attack in TODO 4 |

At camp, choose **1** to explore, **2** to rest, **3** for stats, or **4–6** to visit the unfinished features. In battle, choose **1** to attack or **0** to flee; the potion and special attack options are unfinished.

Choose **7: Save & quit** to keep your hero's progress. **0: Quit without saving** asks for confirmation and discards unsaved hero changes. Starting a new hero also asks for confirmation; it replaces the saved hero only when you later choose **Save & quit**.

## Where the SQL fits

By default, the game creates `dungeon.db` beside its executable. Save data and enemy data live in that file. You can launch the game from another working directory: CMake records where the project's SQL source files are located.

Keep the source folder available when playing. If you move the project, remove its generated `build` folder and rebuild so CMake updates the recorded paths. To reset your hero and enemy data, close the game and rename `dungeon.db` as a backup; the next launch creates a fresh database.

Use a separate database for experiments:

```powershell
.\build\Debug\dungeon_workshop.exe --db .\experiment.db
```

Run these examples from the project folder. Relative paths passed to `--db` and `--mod` are relative to your current working directory.

| File | Purpose |
| --- | --- |
| `sql/schema.sql` | Defines the database tables |
| `sql/seed.sql` | Adds starting enemy data when a database is first created |
| `sql/practice.sql` | SQL exercises to read and try as you learn |
| `sql/mods/gentle_mode.sql` | Example mod that sets enemy attack to 1 |

To try the example mod against your current database:

```powershell
.\build\Debug\dungeon_workshop.exe --mod .\sql\mods\gentle_mode.sql
```

Mods run against the selected database before play, and their changes persist immediately. Quitting without saving does not undo a mod. Apply local SQL that you have read and understand. The gentle mode mod can be applied repeatedly with the same result.

To change the starting enemies, edit `sql/seed.sql`, then launch with a **new database filename**. Existing databases are not reseeded. Editing `schema.sql` also does not change tables that already exist; use a fresh database while practising schema changes.

## Read the code in this order

1. `src/main.cpp` — how the program starts.
2. `src/Models.h` — the hero and enemy data.
3. `src/Game.h` and `src/Game.cpp` — the menus and game loop.
4. `src/Extensions.h` and `src/Extensions.cpp` — your unfinished features.
5. `sql/schema.sql`, `sql/seed.sql`, and `sql/practice.sql` — the SQL side.
6. `src/Input.h` and `src/Input.cpp` — reusable input handling.

`src/Database.h` and `src/Database.cpp` contain the SQLite wrapper. This is the more advanced part: you can leave it alone for your first few exercises and return to it when you want to understand loading, saving, and prepared statements.

Start with [NEXT_STEPS.md](NEXT_STEPS.md). It gives each TODO a small goal and a few checks, without giving you the completed solution. For SQL syntax, use the [official SQLite language reference](https://www.sqlite.org/lang.html).

## Check the starter

After building, run the database smoke test:

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

Then try one fight, return to camp, save, and restart to check that your hero's progress loads. Use a separate `--db` file when experimenting so your main saved hero stays available.
