#include "GameDirector.h"

#include <iostream>

#include "Helper.h"
#include "World.h"
#include "Game.h"
#include "Chest.h"
#include "Enemy.h"


GenericGameQuestion::GenericGameQuestion(GenericGameQuestionType questionType) :
questionType_(questionType)
{
}


GenericGameQuestion::~GenericGameQuestion()
{
}


std::string GenericGameQuestion::GetQuestion() const
{
    switch (questionType_) {
    default:
        return std::string();

    case GenericGameQuestionType::RootDirectoryName:
        return "What is the name given to the / directory?";

    case GenericGameQuestionType::FilesystemGraph:
        return "Which structure best fits the layout of files in a UNIX-style file system?";

    case GenericGameQuestionType::TmpDirectory:
        return "In which directory are temporary files (with a short TTL) located?";

    case GenericGameQuestionType::DevDirectory:
        return "Which types of files are contained within the /dev directory?";

    case GenericGameQuestionType::BinDirectory:
        return "Which types of files are contained within the /bin directory?";

    case GenericGameQuestionType::HomeDirectory:
        return "What is contained within the /home directory?";

    case GenericGameQuestionType::DoubleDot:
        return "Which directory does /bin/.. refer to?";

    case GenericGameQuestionType::SingleDot:
        return "Which directory does /sbin/. refer to?";

    case GenericGameQuestionType::EquivPath:
        return "Are the paths /usr/local/bin and /usr///local//bin equivalent?";
    }
}


std::string GenericGameQuestion::GetAnswerChoice(GameQuestionAnswerChoice choice) const
{
    switch (questionType_) {
    default:
        return std::string();

    case GenericGameQuestionType::RootDirectoryName:
        switch (choice) {
        case GameQuestionAnswerChoice::CorrectChoice:
            return "root";

        case GameQuestionAnswerChoice::WrongChoice1:
            return "slash";

        case GameQuestionAnswerChoice::WrongChoice2:
            return "top";
        }

    case GenericGameQuestionType::FilesystemGraph:
        switch (choice) {
        case GameQuestionAnswerChoice::CorrectChoice:
            return "An inverted tree";

        case GameQuestionAnswerChoice::WrongChoice1:
            return "A matrix";

        case GameQuestionAnswerChoice::WrongChoice2:
            return "An ordered list";
        }

    case GenericGameQuestionType::TmpDirectory:
        switch (choice) {
        case GameQuestionAnswerChoice::CorrectChoice:
            return "/tmp";

        case GameQuestionAnswerChoice::WrongChoice1:
            return "/etc";

        case GameQuestionAnswerChoice::WrongChoice2:
            return "/var";
        }

    case GenericGameQuestionType::DevDirectory:
        switch (choice) {
        case GameQuestionAnswerChoice::CorrectChoice:
            return "Device files";

        case GameQuestionAnswerChoice::WrongChoice1:
            return "Development files";

        case GameQuestionAnswerChoice::WrongChoice2:
            return "Deviation files";
        }

    case GenericGameQuestionType::BinDirectory:
        switch (choice) {
        case GameQuestionAnswerChoice::CorrectChoice:
            return "Executable binary files";

        case GameQuestionAnswerChoice::WrongChoice1:
            return "Junk files";

        case GameQuestionAnswerChoice::WrongChoice2:
            return "Junk binaries";
        }

    case GenericGameQuestionType::HomeDirectory:
        switch (choice) {
        case GameQuestionAnswerChoice::CorrectChoice:
            return "Directories containing files for each specific user";

        case GameQuestionAnswerChoice::WrongChoice1:
            return "Directories containing operating system files";

        case GameQuestionAnswerChoice::WrongChoice2:
            return "Directories containing... homes?";
        }

    case GenericGameQuestionType::DoubleDot:
        switch (choice) {
        case GameQuestionAnswerChoice::CorrectChoice:
            return "/";

        case GameQuestionAnswerChoice::WrongChoice1:
            return "/bin";

        case GameQuestionAnswerChoice::WrongChoice2:
            return "C:\\";
        }

    case GenericGameQuestionType::SingleDot:
        switch (choice) {
        case GameQuestionAnswerChoice::CorrectChoice:
            return "/sbin";

        case GameQuestionAnswerChoice::WrongChoice1:
            return "/";

        case GameQuestionAnswerChoice::WrongChoice2:
            return "D:\\";
        }

    case GenericGameQuestionType::EquivPath:
        switch (choice) {
        case GameQuestionAnswerChoice::CorrectChoice:
            return "Yes";

        case GameQuestionAnswerChoice::WrongChoice1:
            return "No";

        case GameQuestionAnswerChoice::WrongChoice2:
            return "What?!";
        }
    }

    return std::string();
}


GameDirector::GameDirector() :
objectiveFs_(nullptr),
objectiveFsNode_(nullptr),
objective_(GameObjectiveType::NotStarted),
objectiveDifficultyMul_(0.0f),
iActiveQuestion_(0),
activeQuestionResult_(GameQuestionAnswerResult::Unanswered)
{
}


GameDirector::~GameDirector()
{
}


void GameDirector::ResetUnusedQuestionsList()
{
    unusedQuestions_.clear();

    unusedQuestions_.emplace_back(std::make_unique<GenericGameQuestion>(GenericGameQuestionType::RootDirectoryName));
    unusedQuestions_.emplace_back(std::make_unique<GenericGameQuestion>(GenericGameQuestionType::FilesystemGraph));
    unusedQuestions_.emplace_back(std::make_unique<GenericGameQuestion>(GenericGameQuestionType::TmpDirectory));
    unusedQuestions_.emplace_back(std::make_unique<GenericGameQuestion>(GenericGameQuestionType::DevDirectory));
    unusedQuestions_.emplace_back(std::make_unique<GenericGameQuestion>(GenericGameQuestionType::BinDirectory));
    unusedQuestions_.emplace_back(std::make_unique<GenericGameQuestion>(GenericGameQuestionType::HomeDirectory));
    unusedQuestions_.emplace_back(std::make_unique<GenericGameQuestion>(GenericGameQuestionType::DoubleDot));
    unusedQuestions_.emplace_back(std::make_unique<GenericGameQuestion>(GenericGameQuestionType::SingleDot));
    unusedQuestions_.emplace_back(std::make_unique<GenericGameQuestion>(GenericGameQuestionType::EquivPath));

    // invalid i so no question gets removed from the unused list when
    // selecting the first question to be given
    iActiveQuestion_ = unusedQuestions_.size();

    std::cout << "GameDirector - reset unused questions list\n";
}


void GameDirector::StartNewSession(int maxArtefacts, WorldArea* currentArea, GameFilesystem* fs)
{
    std::cout << "GameDirector - Creating new session!\n";

    numArtefacts_ = 0;
    maxArtefacts_ = maxArtefacts;
    objectiveFs_ = fs;
    objective_ = GameObjectiveType::CollectArtefact;
    objectiveDifficultyMul_ = 1.0f;

    ResetUnusedQuestionsList();
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


void GameDirector::RemoveAllEnemies(WorldArea* area)
{
    if (area) {
        auto enemies = area->GetAllEntitiesOfType<Enemy>();

        for (auto id : enemies) {
            auto ent = area->GetEntity<Enemy>(id);
            assert(ent);

            ent->MarkForDeletion();
        }
    }
}


void GameDirector::PopulateWithEnemies(WorldArea* area, float difficultyMul)
{
    if (area && area->GetRelatedNode()) {
        int targetEnemies = Helper::GenerateRandomInt<int>(area->GetRelatedNode()->GetChildrenCount() / 2,
            std::max<int>(3, static_cast<std::size_t>(area->GetRelatedNode()->GetChildrenCount() * 1.4f)));
        std::cout << "Target enemies for floor: " << targetEnemies << "\n";

        // select enemy types that can be spawned
        std::vector<EnemyType> spawnableEnemies;

        if (difficultyMul < 8.0f) {
            spawnableEnemies.emplace_back(EnemyType::GreenBlobBasic);
        }
        if (difficultyMul >= 1.0f) {
            spawnableEnemies.emplace_back(EnemyType::BlueBlobBasic);
        }
        if (difficultyMul >= 2.0f) {
            spawnableEnemies.emplace_back(EnemyType::RedBlobBasic);
        }
        if (difficultyMul >= 3.0f) {
            spawnableEnemies.emplace_back(EnemyType::PinkBlobBasic);
        }
        if (difficultyMul >= 4.0f && difficultyMul < 10.0f) {
            spawnableEnemies.emplace_back(EnemyType::SkeletonBasic);
        }
        if (difficultyMul >= 5.0f) {
            spawnableEnemies.emplace_back(EnemyType::GhostBasic);
        }
        if (difficultyMul >= 6.0f) {
            spawnableEnemies.emplace_back(EnemyType::MagicFlameBasic);
        }
        if (difficultyMul >= 7.0f) {
            spawnableEnemies.emplace_back(EnemyType::AncientWizardBasic);
        }
        if (difficultyMul >= 9.0f) {
            spawnableEnemies.emplace_back(EnemyType::DarkWizardBasic);
        }

        // try spawn enemies
        if (!spawnableEnemies.empty()) {
            for (int i = 0; i < targetEnemies; ++i) {
                int tryCount = 0;

                while (tryCount++ < 10000) {
                    auto desiredArea = sf::FloatRect(
                        Helper::GenerateRandomInt<u32>(0, area->GetWidth() - 1) * BaseTile::TileSize.x,
                        Helper::GenerateRandomInt<u32>(0, area->GetHeight() - 1) * BaseTile::TileSize.y,
                        16.0f, 16.0f
                        );

                    if (area->CheckEntRectangleWalkable(desiredArea)) {
                        auto chosenEnemyType = spawnableEnemies[Helper::GenerateRandomInt<std::size_t>(0,
                            spawnableEnemies.size() - 1)];
                        auto enemyEnt = area->GetEntity<Enemy>(area->EmplaceEntity<BasicEnemy>(chosenEnemyType));

                        if (enemyEnt) {
                            enemyEnt->SetPosition(sf::Vector2f(desiredArea.left, desiredArea.top));
                            break;
                        }
                    }
                }
            }
        }
    }
}


void GameDirector::PlayerChangedArea(WorldArea* newArea, bool updateEnemies)
{
    if (!newArea || !newArea->GetRelatedNode()) {
        return;
    }

    if (std::find(objectiveUpdatedAreas_.begin(), objectiveUpdatedAreas_.end(), newArea) ==
        objectiveUpdatedAreas_.end()) {
        // reopen some of the chests & always reopen the artefact chest.
        auto chestIds = newArea->GetAllEntitiesOfType<ChestEntity>();

        bool isArtefactFloor = (objective_ == GameObjectiveType::CollectArtefact &&
            objectiveFsNode_ && objectiveFsNode_->GetParent() == newArea->GetRelatedNode());
        bool foundArtefactChest = false;
        int numChestsClosed = 0;

        for (auto id : chestIds) {
            auto chestEnt = newArea->GetEntity<ChestEntity>(id);
            assert(chestEnt);

            bool isArtefactChest = isArtefactFloor && objectiveFsNode_->GetName() == chestEnt->GetChestFsNodeName();

            if (chestEnt->IsOpened()) {
                if (isArtefactChest || Helper::GenerateRandomBool(0.5f)) {
                    // close this chest again
                    chestEnt->SetOpened(false);
                    ++numChestsClosed;

                    // spawn an effect here lol
                    auto effect = newArea->GetEntity<DamageEffectEntity>(
                        newArea->EmplaceEntity<DamageEffectEntity>(DamageEffectType::EnemyWave, sf::seconds(0.5f)));
                    assert(effect);
                    
                    effect->SetCenterPosition(chestEnt->GetCenterPosition());
                    GameAssets::Get().drainSound.play();
                }
            }

            if (isArtefactChest) {
                assert(!foundArtefactChest && "Already found an artefact chest on this floor!!!");
                foundArtefactChest = true;
            }
        }

        assert((!isArtefactFloor || foundArtefactChest) && "Artefact chest not found on artefact floor!!!!");

        // announce num chests closed again
        std::cout << "GameDirector - Closed " << numChestsClosed << " chests on this floor.\n";
        if (numChestsClosed > 0) {
            Game::Get().AddMessage(std::to_string(numChestsClosed) + (numChestsClosed > 1 ? " chests have " : " chest has ") +
                "been mysteriously shut again on this floor...", sf::Color(255, 150, 0));
        }

        // populate with enemies again if needed
        if (updateEnemies) {
            RemoveAllEnemies(newArea);
            PopulateWithEnemies(newArea);
        }

        std::cout << "GameDirector - Updated area to reflect the current difficulty of " << objectiveDifficultyMul_ << "\n";
        objectiveUpdatedAreas_.emplace_back(newArea);
    }
}


void GameDirector::SelectNewQuestion()
{
    Game::Get().ResetDisplayedQuestion();

    if (unusedQuestions_.size() <= 1) {
        ResetUnusedQuestionsList();
    }

    if (iActiveQuestion_ < unusedQuestions_.size()) {
        unusedQuestions_.erase(unusedQuestions_.begin() + iActiveQuestion_);
    }

    iActiveQuestion_ = Helper::GenerateRandomInt<std::size_t>(0, unusedQuestions_.size() - 1);
    activeQuestionResult_ = GameQuestionAnswerResult::Unanswered;
}


void GameDirector::AnswerQuestionResult(GameQuestionAnswerResult result, WorldArea* area)
{
    activeQuestionResult_ = result;
    Game::Get().ResetDisplayedQuestion();

    if (GetCurrentQuestion()) {
        if (activeQuestionResult_ == GameQuestionAnswerResult::Correct) {
            std::cout << "GameDirector - Correct question answer! WEW\n";
            Game::Get().AddMessage("Nice - That was the correct answer!", sf::Color(0, 255, 0));
            Game::Get().AddMessage("You hear a click from the locking mechanism within the chest.", sf::Color(0, 255, 0));
        }
        else if (activeQuestionResult_ == GameQuestionAnswerResult::Wrong) {
            std::cout << "GameDirector - Incorrect question answer! :(\n";
            Game::Get().AddMessage("Sorry - that answer is incorrect!", sf::Color(255, 0, 0));

            auto correctAnswer = GetCurrentQuestion()->GetAnswerChoice(GameQuestionAnswerChoice::CorrectChoice);
            Game::Get().AddMessage("The correct answer was '" + correctAnswer + "'", sf::Color(255, 0, 0));

            Game::Get().AddMessage("The artefact piece has relocated elsewhere...", sf::Color(255, 0, 0));

            if (objective_ == GameObjectiveType::CollectArtefact) {
                ChooseNewArtefactLocation(area);
            }
        }
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

    
    std::cout << "GameDirector - Reservoir Sampling; " << result.second << " file nodes considered.\n";

    objectiveFsNodePath_ = GameFilesystem::GetNodePathString(*objectiveFsNode_);

    std::cout << "GameDirector - NEW ARTEFACT LOCATION: '" << objectiveFsNodePath_ << "'\n";
    Game::Get().AddMessage("An artefact piece is in the " + objectiveFsNodePath_ + " chest!",
        sf::Color(255, 150, 0));

    ResetObjective();
    SelectNewQuestion();
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

    std::cout << "GameDirector - NEW OBJECTIVE: difficulty " << objectiveDifficultyMul_ << "\n";
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

            std::cout << "GameDirector - ALL ARTEFACTS FOUND!\n";
            Game::Get().AddMessage("Well done! You have found all " + std::to_string(maxArtefacts_) + " artefact pieces.",
                sf::Color(255, 150, 0));
            Game::Get().AddMessage("Mysterious stairs have appeared within the gilded room on /",
                sf::Color(255, 0, 255));
            Game::Get().AddMessage("Maybe it's the dungeon exit? You should investigate...", sf::Color(255, 0, 255));
        }
    }
}


void GameDirector::ReleaseTheBoss(WorldArea* spawnArea, const sf::Vector2f& pos)
{
    if (!spawnArea) {
        return;
    }

    objective_ = GameObjectiveType::BossFight;

    std::cout << "GameDirector - RELEASING THE BOSS!!!!\n";
    Game::Get().AddMessage("The gilded stairs are suddenly engulfed in a magical flame!", sf::Color(255, 0, 0));
    Game::Get().AddMessage("The Dungeon Guardian has been awoken!", sf::Color(255, 0, 0));
    
    auto boss = spawnArea->GetEntity<DungeonGuardian>(spawnArea->EmplaceEntity<DungeonGuardian>());
    assert(boss);

    boss->SetPosition(pos);
}


void GameDirector::PlayerKilled(WorldArea* area)
{
    if (!area || objective_ != GameObjectiveType::BossFight) {
        return;
    }

    std::cout << "GameDirector - notify bosses player killed\n";

    auto bosses = area->GetAllEntitiesOfType<DungeonGuardian>();

    for (auto bossId : bosses) {
        auto boss = area->GetEntity<DungeonGuardian>(bossId);
        assert(boss);

        if (boss->GetStats() && boss->GetStats()->IsAlive()) {
            boss->PlayerKilled();
        }
    }
}


void GameDirector::BossDefeated()
{
    if (objective_ != GameObjectiveType::BossFight) {
        return;
    }

    objective_ = GameObjectiveType::Complete;

    std::cout << "GameDirector - BOSS DEFEATED!!!!\n";
    Game::Get().AddMessage("Congratulations! The Dungeon Guardian has been defeated!", sf::Color(255, 150, 0));
    Game::Get().AddMessage("The magical flames blocking the gilded stairs have been doused!", sf::Color(255, 0, 255));
    Game::Get().AddMessage("Ascend the mysterious stairs to finally exit the dungeon!", sf::Color(255, 0, 255));

    GameAssets::Get().successSound.play();
}


void GameDirector::EndGame()
{
    if (objective_ == GameObjectiveType::End) {
        return;
    }

    objective_ = GameObjectiveType::End;

    std::cout << "GameDirector - Game ended!\n";
    Game::Get().AddMessage("Well done - you win!");

    GameAssets::Get().invincibilitySound.play();
}


std::string GameDirector::GetObjectiveText() const
{
    switch (objective_) {
    default:
        std::string();

    case GameObjectiveType::NotStarted:
    case GameObjectiveType::End:
        return "No objective.";

    case GameObjectiveType::Complete:
        return "Ascend the mysterious staircase in the gilded room to exit the dungeon!";

    case GameObjectiveType::CollectArtefact:
        if (!objectiveFsNode_) {
            return std::string();
        }

        if (numArtefacts_ < maxArtefacts_) {
            return "Collect artefact piece " + std::to_string(numArtefacts_ + 1) + " of " +
                std::to_string(maxArtefacts_) + " in the " + objectiveFsNodePath_ + " chest.";
        }

        return "Collect artefact piece " + std::to_string(numArtefacts_ + 1) + " in the " + objectiveFsNodePath_ +
            "chest.";

    case GameObjectiveType::RootArtefactAltar:
        return "Investigate the mysterious stairs that appeared within the gilded room on the / floor.";

    case GameObjectiveType::BossFight:
        return "Defeat the Dungeon Guardian!";
    }
}