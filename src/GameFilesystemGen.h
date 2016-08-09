#pragma once

#include <random>
#include <memory>
#include <array>
#include <chrono>

#include "Types.h"
#include "GameFilesystem.h"
#include "Helper.h"

/**
* Procedurally generates a UNIX-style virtual filesystem for gameplay
*/
class GameFilesystemGen
{
    static const std::array<std::string, 45> fileNameGenPhrases_;
    static const std::array<std::string, 40> userNames_;

    static inline std::string GenerateRandomHexName(Rng& rng, int numChars)
    {
        const static std::string hexChars = "0123456789abcdef";

        std::string result;
        for (int i = 0; i < numChars; ++i) {
            result += hexChars[Helper::GenerateRandomInt<Rng, std::size_t>(rng, 0, hexChars.size() - 1)];
        }

        return result;
    }

    static inline std::string GenerateRandomPhraseName(Rng& rng, int maxPhrases = 2)
    {
        if (maxPhrases <= 0) {
            return std::string();
        }

        assert(fileNameGenPhrases_.size() > 0);

        bool underscoresBetweenPhrases = Helper::GenerateRandomBool(rng, 0.5);
        int numPhrases = Helper::GenerateRandomInt(rng, 1, maxPhrases);
        std::string result;

        for (int i = 0; i < numPhrases; ++i) {
            auto phraseIdx = Helper::GenerateRandomInt<Rng, std::size_t>(rng, 0, fileNameGenPhrases_.size() - 1);
            result += fileNameGenPhrases_[phraseIdx];

            if (underscoresBetweenPhrases && i != numPhrases - 1) {
                result += '_';
            }
        }

        return result;
    }

	RngInt seed_;

	inline RngInt GenerateNewId(Rng& rng) { return rng(); }

    GameFilesystemNode* AddNodeWithUniqueName(GameFilesystemNode& parent, std::unique_ptr<GameFilesystemNode>& node);
    void GenerateRandomFiles(GameFilesystemNode& parent, Rng& rng, std::size_t numFiles);

    std::size_t GenRandomFileDirsRecurse(GameFilesystemNode* node, Rng& rng, std::size_t numDirsLeft,
        bool firstDir = true);

    void GenerateUsrDir(GameFilesystemNode& parent, Rng& rng);
    void GenerateHomeDir(GameFilesystemNode& parent, Rng& rng);
    void GenerateDevDir(GameFilesystemNode& parent, Rng& rng);
    void GenerateBinDir(GameFilesystemNode& parent, Rng& rng);
    void GenerateSbinDir(GameFilesystemNode& parent, Rng& rng);
    void GenerateEtcDir(GameFilesystemNode& parent, Rng& rng);
    void GenerateProcDir(GameFilesystemNode& parent, Rng& rng);
    void GenerateTmpDir(GameFilesystemNode& parent, Rng& rng);
    void GenerateLibDir(GameFilesystemNode& parent, Rng& rng);
    void GenerateMntDir(GameFilesystemNode& parent, Rng& rng);
    void GenerateMediaDir(GameFilesystemNode& parent, Rng& rng);

public:
    GameFilesystemGen(RngInt seed = static_cast<RngInt>(std::chrono::system_clock::now().time_since_epoch().count()));
	~GameFilesystemGen();

	std::unique_ptr<GameFilesystem> GenerateNewFilesystem();
};

