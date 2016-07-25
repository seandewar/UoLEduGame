#pragma once

#include <string>

#include "GameFilesystem.h"

/**
* Objective types.
*/
enum class GameObjectiveType
{
    NotStarted,
    CollectArtefact,
    RootArtefactAltar,
    BossFight,
    Complete
};

class WorldArea;

/**
* Directs the objectives of the player
*/
class GameDirector
{
    GameObjectiveType objective_;

    float objectiveDifficultyMul_;
    GameFilesystem* objectiveFs_;
    GameFilesystemNode* objectiveFsNode_;
    std::string objectiveFsNodePath_;
    int numArtefacts_, maxArtefacts_;

    std::vector<WorldArea*> objectiveUpdatedAreas_;

    /**
    * Uses the Reservoir Sampling algorithm to pick a random node from the tree.
    */
    std::pair<GameFilesystemNode*, std::size_t> ChooseRandomNodeRecurse(GameFilesystemNode* node, std::size_t n = 1);

    void ChooseNewArtefactLocation();

    void NewObjective(float newDifficultyMul);

public:
    GameDirector();
    ~GameDirector();

    void StartNewSession(int maxArtefacts, GameFilesystem* fs);
    void FoundArtefact();

    void PlayerChangedArea(WorldArea* newArea);

    inline int GetNumArtefacts() const { return numArtefacts_; }
    inline int GetMaxArtefacts() const { return maxArtefacts_; }

    inline GameObjectiveType GetCurrentObjectiveType() const { return objective_; }
    inline float GetCurrentDifficultyMultiplier() const { return objectiveDifficultyMul_; }

    std::string GetObjectiveText() const;
};
