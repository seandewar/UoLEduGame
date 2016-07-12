#pragma once

#include <random>
#include <memory>
#include <array>

#include "Types.h"
#include "GameFilesystem.h"


/**
* Procedurally generates a UNIX-style virtual filesystem for gameplay
*/
class GameFilesystemGen
{
    static const std::array<std::string, 45> fileNameGenPhrases_;

	RngInt seed_;

	inline RngInt GenerateNewId(Rng& rng) { return rng(); }

    void GenerateRandomFiles(GameFilesystemNode& parent, Rng& rng, std::size_t numFiles);

    void GenerateUsrDir(GameFilesystemNode& parent, Rng& rng);
    void GenerateHomeDir(GameFilesystemNode& parent, Rng& rng);

public:
	GameFilesystemGen(RngInt seed);
	~GameFilesystemGen();

	std::unique_ptr<GameFilesystem> GenerateNewFilesystem();
};

