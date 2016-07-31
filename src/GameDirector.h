#pragma once

#include <string>

#include "GameFilesystem.h"

/**
* Answer choice number for GameQuestion
*/
enum class GameQuestionAnswerChoice
{
    CorrectChoice,
    WrongChoice1,
    WrongChoice2
};

/**
* Question class for collecting artefacts
*/
class IGameQuestion
{
public:
    IGameQuestion() { }
    virtual ~IGameQuestion() { }
    
    virtual std::string GetQuestion() const = 0;
    virtual std::string GetAnswerChoice(GameQuestionAnswerChoice choice) const = 0;
};

/**
* Type of GenericGameQuestion
*/
enum class GenericGameQuestionType
{
    RootDirectoryName,
    FilesystemGraph,
    TmpDirectory,
    DevDirectory,
    BinDirectory,
    HomeDirectory,
    DoubleDot,
    SingleDot,
    EquivPath
};

/**
* Generic game questions that do not rely on current world state
*/
class GenericGameQuestion : public IGameQuestion
{
    GenericGameQuestionType questionType_;

public:
    GenericGameQuestion(GenericGameQuestionType questionType);
    virtual ~GenericGameQuestion();

    inline GenericGameQuestionType GetGenericQuestionType() const { return questionType_; }

    virtual std::string GetQuestion() const override;
    virtual std::string GetAnswerChoice(GameQuestionAnswerChoice choice) const override;
};

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

/**
* The result of the player answering the current question.
*/
enum class GameQuestionAnswerResult
{
    Unanswered,
    Correct,
    Wrong
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

    std::vector<std::unique_ptr<IGameQuestion>> unusedQuestions_;
    std::size_t iActiveQuestion_;
    GameQuestionAnswerResult activeQuestionResult_;

    /**
    * Uses the Reservoir Sampling algorithm to pick a random node from the tree.
    */
    std::pair<GameFilesystemNode*, std::size_t> ChooseRandomNodeRecurse(GameFilesystemNode* node, std::size_t n = 0);

    void NewObjective(float newDifficultyMul);
    inline void ResetObjective() { NewObjective(objectiveDifficultyMul_); }

    void ResetUnusedQuestionsList();
    void SelectNewQuestion();

public:
    GameDirector();
    ~GameDirector();

    void RemoveAllEnemies(WorldArea* area);
    void PopulateWithEnemies(WorldArea* area, float difficultyMul);
    inline void PopulateWithEnemies(WorldArea* area) { PopulateWithEnemies(area, objectiveDifficultyMul_); }

    void StartNewSession(int maxArtefacts, WorldArea* currentArea, GameFilesystem* fs);
    void FoundArtefact(WorldArea* currentArea);
    void ChooseNewArtefactLocation(WorldArea* newArea);

    inline GameFilesystemNode* GetCurrentArtefactNode()
    { 
        return objective_ == GameObjectiveType::CollectArtefact ? objectiveFsNode_ : nullptr;
    }

    inline GameQuestionAnswerResult GetQuestionAnswerResult() const { return activeQuestionResult_; }
    void AnswerQuestionResult(GameQuestionAnswerResult result, WorldArea* area);

    inline const IGameQuestion* GetCurrentQuestion() const
    { 
        return iActiveQuestion_ < unusedQuestions_.size() ? unusedQuestions_[iActiveQuestion_].get() : nullptr;
    }

    void PlayerChangedArea(WorldArea* newArea, bool updateEnemies = true);

    inline int GetNumArtefacts() const { return numArtefacts_; }
    inline int GetMaxArtefacts() const { return maxArtefacts_; }

    inline GameObjectiveType GetCurrentObjectiveType() const { return objective_; }
    inline float GetCurrentDifficultyMultiplier() const { return objectiveDifficultyMul_; }

    std::string GetObjectiveText() const;
};

