#include "GameDirector.h"

#include "Helper.h"
#include "World.h"
#include "Game.h"


GameDirector::GameDirector() :
objectiveFs_(nullptr),
objectiveFsNode_(nullptr),
objective_(GameObjectiveType::NotStarted),
objectiveDifficultyMul_(0.0f)
{
}


GameDirector::~GameDirector()
{
}


void GameDirector::StartNewSession(int maxArtefacts, GameFilesystem* fs)
{
    printf("GameDirector - Creating new session!\n");

    numArtefacts_ = 0;
    maxArtefacts_ = maxArtefacts;
    objectiveFs_ = fs;
    objective_ = GameObjectiveType::CollectArtefact;
    objectiveDifficultyMul_ = 1.0f;

    ChooseNewArtefactLocation();
}


std::pair<GameFilesystemNode*, std::size_t> GameDirector::ChooseRandomNodeRecurse(GameFilesystemNode* node,
    std::size_t n)
{
    if (!node) {
        // error - null node; recurse down with an invalid n (n <= 0)
        return std::make_pair(nullptr, 0);
    }

    auto result = std::make_pair(!node->IsDirectory() && Helper::GenerateRandomBool(1.0f / n) ? node : nullptr, n);

    for (std::size_t iNode = 0; iNode < node->GetChildrenCount(); ++iNode) {
        auto nodeResult = ChooseRandomNodeRecurse(node->GetChildNode(iNode), ++result.second);

        if (nodeResult.second <= 0) {
            // error - recurse down with an invalid n (n <= 0)
            return std::make_pair(nullptr, 0);
        }

        // check if any node down this branch was selected
        if (nodeResult.first) {
            result.first = nodeResult.first;
        }

        // update current n
        result.second = nodeResult.second;
    }

    // if this is the bottom-most recursive call, print dbg
    if (n == 1) {
        printf("GameDirector - ChooseRandomNodeRecurse(): Reservoir Sampling; %u nodes considered.\n", result.second);
    }

    return result;
}


void GameDirector::PlayerChangedArea(WorldArea* newArea)
{
    if (!newArea || !newArea->GetRelatedNode()) {
        return;
    }

    if (std::find(objectiveUpdatedAreas_.begin(), objectiveUpdatedAreas_.end(), newArea) ==
        objectiveUpdatedAreas_.end()) {
        // TODO clear monsters & update area monster difficulty & 
        // place artefact in chest if objective node associated

        printf("GameDirector - Updated area to reflect the current difficulty of %f\n", objectiveDifficultyMul_);
        objectiveUpdatedAreas_.emplace_back(newArea);
    }
}


void GameDirector::ChooseNewArtefactLocation()
{
    if (objective_ != GameObjectiveType::CollectArtefact || !objectiveFs_ || !objectiveFs_->GetRootNode()) {
        return;
    }

    // pick a random node in the fs
    objectiveFsNode_ = ChooseRandomNodeRecurse(objectiveFs_->GetRootNode()).first;
    assert(objectiveFsNode_);

    objectiveFsNodePath_ = GameFilesystem::GetNodePathString(*objectiveFsNode_);

    printf("GameDirector - NEW ARTEFACT LOCATION: '%s'\n", objectiveFsNodePath_.c_str());
    Game::Get().AddMessage("An artefact piece is in the " + objectiveFsNodePath_ + " chest!",
        sf::Color(255, 150, 0));
}


void GameDirector::NewObjective(float newDifficultyMul)
{
    // spoop the player
    if (newDifficultyMul > objectiveDifficultyMul_) {
        switch (Helper::GenerateRandomInt(0, 6)) {
        case 0:
            Game::Get().AddMessage("You feel like you are being watched...", sf::Color(255, 0, 0));
            break;

        case 1:
            Game::Get().AddMessage("The dark energy of the dungeon increases...", sf::Color(255, 0, 0));
            break;

        case 2:
            Game::Get().AddMessage("The air suddenly feels thicker...", sf::Color(255, 0, 0));
            break;

        case 3:
            Game::Get().AddMessage("Turn back...", sf::Color(255, 0, 0));
            break;

        case 4:
            Game::Get().AddMessage("You don't belong here...", sf::Color(255, 0, 0));
            break;

        case 5:
            Game::Get().AddMessage("What was that!?", sf::Color(255, 0, 0));
            break;

        case 6:
            Game::Get().AddMessage("That couldn't have been good...", sf::Color(255, 0, 0));
            break;
        }
    }

    objectiveDifficultyMul_ = newDifficultyMul;
    objectiveUpdatedAreas_.clear();

    printf("GameDirector - NEW OBJECTIVE: difficulty %f\n", objectiveDifficultyMul_);
}


void GameDirector::FoundArtefact()
{
    ++numArtefacts_;

    if (objective_ == GameObjectiveType::CollectArtefact) {
        if (numArtefacts_ < maxArtefacts_) {
            ChooseNewArtefactLocation();
        }
        else {
            // all artefacts found, next objective is altar in /
            objective_ = GameObjectiveType::RootArtefactAltar;
            printf("GameDirector - ALL ARTEFACTS FOUND!\n");
        }

        NewObjective(objectiveDifficultyMul_ + 1.0f);
    }
}


std::string GameDirector::GetObjectiveText() const
{
    switch (objective_) {
    default:
        std::string();

    case GameObjectiveType::NotStarted:
        return "No objective.";

    case GameObjectiveType::Complete:
        return "Objectives complete!";

    case GameObjectiveType::CollectArtefact:
        if (!objectiveFsNode_) {
            return std::string();
        }

        if (numArtefacts_ < maxArtefacts_) {
            return "Collect artefact piece " + std::to_string(numArtefacts_ + 1) + " of " +
                std::to_string(maxArtefacts_) + " in " + objectiveFsNodePath_;
        }

        return "Collect artefact piece " + std::to_string(numArtefacts_ + 1) + " in " + objectiveFsNodePath_;

    case GameObjectiveType::RootArtefactAltar:
        return "Place your artefact pieces on the altar in /";

    case GameObjectiveType::BossFight:
        return "Reign victorious in your final battle!";
    }
}