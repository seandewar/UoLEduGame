# minimum cmake version
cmake_minimum_required(VERSION 3.5)

# project name
project(UoLEduGame)

# c++11 standard required
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# include dir for searching for .h files and libs
include_directories(include)
link_directories(lib)

# add sources to executable
# (make sure to also add the headers of project so they appear in project file)
add_executable(UoLEduGame
	src/Types.h
	src/Helper.h
	src/Helper.cpp
	src/GameFilesystemGen.h
	src/GameFilesystemGen.cpp
	src/GameFilesystem.h
	src/GameFilesystem.cpp
    src/Collision.h
    src/Collision.cpp
    src/Animation.h
    src/Animation.cpp
    src/Entity.h
    src/Entity.cpp
    src/PlayerUsable.h
    src/PlayerFacingDirection.h
    src/Player.h
    src/Player.cpp
    src/Enemy.h
    src/Enemy.cpp
    src/Projectile.h
    src/Projectile.cpp
    src/Item.h
    src/Item.cpp
    src/Chest.h
    src/Chest.cpp
    src/Altar.h
    src/Altar.cpp
    src/Stairs.h
    src/Stairs.cpp
	src/World.h
	src/World.cpp
	src/Tile.h
	src/Tile.cpp
	src/DungeonGen.h
	src/DungeonGen.cpp
	src/Game.h
	src/Game.cpp
    src/GameDirector.h
    src/GameDirector.cpp
	src/main.cpp
	)

# first search for cmake module files in the local cmake dir
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

# find SFML
find_package(SFML 2 REQUIRED system window graphics audio)
if (SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(UoLEduGame ${SFML_LIBRARIES})
endif()

# copy assets to binary dir
file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/assets)
file(INSTALL assets DESTINATION ${CMAKE_BINARY_DIR})

# install system and compiler runtimes .etc
include(InstallRequiredSystemLibraries)

# install target
install(TARGETS UoLEduGame DESTINATION ${CMAKE_BINARY_DIR}/bin)
