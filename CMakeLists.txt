cmake_minimum_required(VERSION 3.20)
project(DungeonWorkshop LANGUAGES CXX)

# CMake turns these source files into an executable. No game engine is needed.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(game_database src/Database.cpp)
target_include_directories(game_database PUBLIC src)

if(WIN32)
    # Windows 10/11 already provides SQLite; the Windows SDK supplies its header
    # and import library. Install Visual Studio's C++ workload if either is missing.
    target_compile_definitions(game_database PRIVATE _WIN32_WINNT=0x0A00)
    target_link_libraries(game_database PUBLIC winsqlite3)
else()
    find_package(SQLite3 REQUIRED)
    target_link_libraries(game_database PUBLIC SQLite::SQLite3)
endif()

add_executable(dungeon_workshop
    src/main.cpp
    src/Game.cpp
    src/Input.cpp
    src/Extensions.cpp
)
target_link_libraries(dungeon_workshop PRIVATE game_database)
# This lets the game find your editable SQL even when launched from another folder.
target_compile_definitions(dungeon_workshop PRIVATE
    GAME_SQL_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/sql"
    GAME_BINARY_DIRECTORY="$<TARGET_FILE_DIR:dungeon_workshop>"
)
set_property(TARGET dungeon_workshop PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

foreach(target game_database dungeon_workshop)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()

# A small real-database check, with no external testing framework.
include(CTest)
if(BUILD_TESTING)
    add_executable(database_smoke tests/database_smoke.cpp)
    target_link_libraries(database_smoke PRIVATE game_database)
    target_compile_definitions(database_smoke PRIVATE
        GAME_SQL_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/sql"
    )
    add_test(NAME database_smoke COMMAND database_smoke)
endif()
