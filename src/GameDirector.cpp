#include "GameDirector.h"

#include "Helper.h"
#include "World.h"
#include "Game.h"
#include "Chest.h"


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


void GameDirector::StartNewSession(int maxArtefacts, WorldArea* currentArea, GameFilesystem* fs)
{
    printf("GameDirector - Creating new session!\n");

    numArtefacts_ = 0;
    maxArtefacts_ = maxArtefacts;
    objectiveFs_ = fs;
    objective_ = GameObjectiveType::CollectArtefact;
    objectiveDifficultyMul_ = 1.0f;

    ChooseNewArtefactLocation(currentArea);
}


std::pair<GameFilesystemNode*, std::size_t> GameDirector::ChooseRandomNodeRecurse(GameFilesystemNode* node,
    std::size_t n)
{
    if (!node) {
        return std::make_pair(nullptr, 0);
    }

    std::pair<GameFilesystemNode*, std::size_t> result = std::make_pair(nullptr, n);
    if (!node->IsDirectory()) {
        // if this is a file node, consider it
        result.second = n + 1;

        // now roll with probability 1/n to have this node as a candidate to return
        // - this is mathematically proven to consider all nodes in the tree in a
        // uniform random manner.
        if (Helper::GenerateRandomBool(1.0f / result.second)) {
            result.first = node;
        }
    }

    for (std::size_t iNode = 0; iNode < node->GetChildrenCount(); ++iNode) {
        auto childNode = node->GetChildNode(iNode);
        auto nodeResult = ChooseRandomNodeRecurse(childNode, result.second);

        // check if any node down this branch was selected
        if (nodeResult.first) {
            result.first = nodeResult.first;
        }

        // update current n
        result.second = nodeResult.second;
    }

    return result;
}


void GameDirector::PlayerChangedArea(WorldArea* newArea, bool updateEnemies)
{
    if (!newArea || !newArea->GetRelatedNode()) {
        return;
    }

    if (std::find(objectiveUpdatedAreas_.begin(), objectiveUpdatedAreas_.end(), newArea) ==
        objectiveUpdatedAreas_.end()) {
        if (updateEnemies) {
            // TODO update enemies & clear or whatever
        }

        if (objective_ == GameObjectiveType::CollectArtefact &&
            objectiveFsNode_->GetParent() == newArea->GetRelatedNode()) {
            // TODO update artefact chest with artefact
            auto chests = newArea->GetAllEntitiesOfType<ChestEntity>();

            bool artefactPlaced = false;
            for (auto chestId : chests) {
                auto chestEnt = newArea->GetEntity<ChestEntity>(chestId);
                assert(chestEnt);

                if (chestEnt->GetChestFsNodeName() == objectiveFsNode_->GetName()) {
                    // found the chest - place artefact inside and close it if it's already open
                    chestEnt->SetOpened(false);

                    ArtefactType artefactType;

                    switch (Helper::GenerateRandomInt(1, 5)) {
                    default:
                    case 1:
                        artefactType = ArtefactType::Appearance1;
                        break;

                    case 2:
                        artefactType = ArtefactType::Appearance2;
                        break;

                    case 3:
                        artefactType = ArtefactType::Appearance3;
                        break;

                    case 4:
                        artefactType = ArtefactType::Appearance4;
                        break;

                    case 5:
                        artefactType = ArtefactType::Appearance5;
                        break;
                    }

                    chestEnt->GetItems().emplace_back(std::make_unique<ArtefactItem>(artefactType));
                    artefactPlaced = true;
                }
            }

            assert(artefactPlaced);
        }

        printf("GameDirector - Updated area to reflect the current difficulty of %f\n", objectiveDifficultyMul_);
        objectiveUpdatedAreas_.emplace_back(newArea);
    }
}


void GameDirector::ChooseNewArtefactLocation(WorldArea* newArea)
{
    if (objective_ != GameObjectiveType::CollectArtefact || !objectiveFs_ || !objectiveFs_->GetRootNode()) {
        return;
    }

    // pick a random node in the fs
    auto result = ChooseRandomNodeRecurse(objectiveFs_->GetRootNode());
    objectiveFsNode_ = result.first;
    assert(objectiveFsNode_);

    printf("GameDirector - Reservoir Sampling; %u file nodes considered.\n", result.second);

    objectiveFsNodePath_ = GameFilesystem::GetNodePathString(*objectiveFsNode_);

    printf("GameDirector - NEW ARTEFACT LOCATION: '%s'\n", objectiveFsNodePath_.c_str());
    Game::Get().AddMessage("An artefact piece is in the " + objectiveFsNodePath_ + " chest!",
        sf::Color(255, 150, 0));

    PlayerChangedArea(newArea, false);
}


void GameDirector::NewObjective(float newDifficultyMul)
{
    // spoop the player
    if (newDifficultyMul > objectiveDifficultyMul_) {
        switch (Helper::GenerateRandomInt(0, 7)) {
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

        case 7:
            Game::Get().AddMessage("You have a bad feeling about this...", sf::Color(255, 0, 0));
            break;
        }
    }

    objectiveDifficultyMul_ = newDifficultyMul;
    objectiveUpdatedAreas_.clear();

    printf("GameDirector - NEW OBJECTIVE: difficulty %f\n", objectiveDifficultyMul_);
}


void GameDirector::FoundArtefact(WorldArea* currentArea)
{
    ++numArtefacts_;

    if (objective_ == GameObjectiveType::CollectArtefact) {
        NewObjective(objectiveDifficultyMul_ + 1.0f);

        if (numArtefacts_ < maxArtefacts_) {
            ChooseNewArtefactLocation(currentArea);
        }
        else {
            // all artefacts found, next objective is altar in /
            objective_ = GameObjectiveType::RootArtefactAltar;

            printf("GameDirector - ALL ARTEFACTS FOUND!\n");
            Game::Get().AddMessage("Well done! You have found all " + std::to_string(maxArtefacts_) + " artefact pieces!",
                sf::Color(255, 150, 0));
            Game::Get().AddMessage("Return to / and place your artefact pieces on the altar.", sf::Color(255, 150, 0));
        }
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