#include "GameFilesystemGen.h"

#include <cassert>
#include <sstream>

#include "Helper.h"


/**
* For use inside of the GameFilesystemNode::AddChildNode() or GameFilesystem::AddRootNode() calls.
*/
#define NEW_NODE(name, nodeType, rng) \
	std::make_unique<GameFilesystemNode>(name, GameFilesystemNodeType::nodeType, GenerateNewId(rng))


const std::array<std::string, 45> GameFilesystemGen::fileNameGenPhrases_ = {
    {
        "fire",
        "chicken",
        "file",
        "it",
        "computer",
        "browse",
        "and",
        "yes",
        "level",
        "daemon",
        "text",
        "or",
        "heu",
        "no",
        "system",
        "important",
        "eeee",
        "web",
        "game",
        "dipper",
        "bread",
        "random",
        "document",
        "settings",
        "manager",
        "cheese",
        "mutagen",
        "sign",
        "griffin",
        "cockatrice",
        "roach",
        "geralt",
        "alien",
        "normandy",
        "shepard",
        "controller",
        "dog",
        "matrix",
        "name",
        "calc",
        "null",
        "1000",
        "400",
        "v2",
        "2000"
    }
};


GameFilesystemGen::GameFilesystemGen(RngInt seed) :
seed_(seed)
{
}


GameFilesystemGen::~GameFilesystemGen()
{
}


void GameFilesystemGen::GenerateRandomFiles(GameFilesystemNode& parent, Rng& rng, std::size_t numFiles)
{
    for (std::size_t i = 0; i < numFiles; ++i) {
        std::string nodeName, nodeNameExt;
        GameFilesystemNodeType nodeType;

        // roll what type of file to generate
        switch (Helper::GenerateRandomInt(rng, 0, 2)) {
        case 0:
            // generate a text file with .log extension consisting of numbers
            // in the name

            nodeName = std::to_string(Helper::GenerateRandomInt(rng, 0, 99999));
            nodeNameExt = ".log";
            nodeType = GameFilesystemNodeType::TextFile;
            break;

        case 1:
        case 2:
            // generate other file with name using phrases list

            assert(fileNameGenPhrases_.size() > 0);

            {
                bool underscoresBetweenPhrases = Helper::GenerateRandomBool(rng, 0.5);
                int numPhrases = Helper::GenerateRandomInt(rng, 1, 3);

                for (int i = 0; i < numPhrases; ++i) {
                    auto phraseIdx = Helper::GenerateRandomInt<Rng, std::size_t>(rng, 0, fileNameGenPhrases_.size() - 1);
                    nodeName += fileNameGenPhrases_[phraseIdx];

                    if (underscoresBetweenPhrases && i != numPhrases - 1) {
                        nodeName += '_';
                    }
                }
            }

            // choose a filetype
            switch (Helper::GenerateRandomInt(rng, 0, 10)) {
            case 0:
            case 1:
                nodeNameExt = ".txt";
                nodeType = GameFilesystemNodeType::TextFile;
                break;

            case 2:
            case 3:
                nodeNameExt = ".o";
                nodeType = GameFilesystemNodeType::LibraryFile;
                break;

            case 4:
            case 5:
                nodeNameExt = "";
                nodeType = GameFilesystemNodeType::ExecutableFile;
                break;

            case 6:
            case 7:
                nodeNameExt = ".sh";
                nodeType = GameFilesystemNodeType::ShellScriptFile;
                break;

            case 8:
                nodeNameExt = ".png";
                nodeType = GameFilesystemNodeType::ImageFile;
                break;

            case 9:
                nodeNameExt = ".jpg";
                nodeType = GameFilesystemNodeType::ImageFile;
                break;

            case 10:
                nodeNameExt = ".gif";
                nodeType = GameFilesystemNodeType::ImageFile;
                break;

            default:
                assert(!"Unknown random file type to be generated!!");
                return;
            }
            break;

        default:
            assert(!"Unknown random file type to be generated.");
            return;
        }

        // attempt to add the file node with this name.
        // if the name already exists, try with a new unique name by appending
        // a number at the end of it (before the extension)
        auto fileNode = std::make_unique<GameFilesystemNode>(nodeName + nodeNameExt, nodeType, GenerateNewId(rng));

        std::size_t renameTryCount = 0;
        while (true) {
            try {
                parent.AddChildNode(fileNode);
                break;
            }
            catch (const GameFilesystemException& e) {
                if (e.GetType() != GameFilesystemExceptionType::NameIsTaken) {
                    // re-throw if it's not a NameIsTaken exception type
                    throw;
                }
            }

            ++renameTryCount;

            std::ostringstream oss;
            oss << nodeName << '(' << renameTryCount << ')' << nodeNameExt;
            fileNode->SetName(oss.str());
        }
    }
}


void GameFilesystemGen::GenerateUsrDir(GameFilesystemNode& parent, Rng& rng)
{
	auto usrNode = parent.AddChildNode(NEW_NODE("usr", Directory, rng));

	// TODO: Generate files in these directories
	usrNode->AddChildNode(NEW_NODE("bin", Directory, rng));
	usrNode->AddChildNode(NEW_NODE("lib", Directory, rng));
	usrNode->AddChildNode(NEW_NODE("share", Directory, rng));
	usrNode->AddChildNode(NEW_NODE("include", Directory, rng));
}


void GameFilesystemGen::GenerateHomeDir(GameFilesystemNode& parent, Rng& rng)
{
	auto homeNode = parent.AddChildNode(NEW_NODE("home", Directory, rng));

	// TODO: Generate users
}


std::unique_ptr<GameFilesystem> GameFilesystemGen::GenerateNewFilesystem()
{
	Rng rng(seed_);
	auto fs = std::make_unique<GameFilesystem>();

	// create root dir
	auto rootNode = fs->SetRootNode(NEW_NODE("root", RootDirectory, rng));
    assert(rootNode);

	// populate root dir
    // TODO POPULATE WITH FILES & FOLDERS THAT FIT THE DIRECTORY'S PURPOSE!!!111
    GenerateUsrDir(*rootNode, rng);
	rootNode->AddChildNode(NEW_NODE("dev", Directory, rng));
	rootNode->AddChildNode(NEW_NODE("etc", Directory, rng));
	rootNode->AddChildNode(NEW_NODE("lib", Directory, rng));
	rootNode->AddChildNode(NEW_NODE("boot", Directory, rng));
	GenerateHomeDir(*rootNode, rng);
	rootNode->AddChildNode(NEW_NODE("mnt", Directory, rng));
	rootNode->AddChildNode(NEW_NODE("proc", Directory, rng));
	rootNode->AddChildNode(NEW_NODE("tmp", Directory, rng));
	rootNode->AddChildNode(NEW_NODE("var", Directory, rng));
	rootNode->AddChildNode(NEW_NODE("sbin", Directory, rng));
	rootNode->AddChildNode(NEW_NODE("kernel", Directory, rng));

    GenerateRandomFiles(*rootNode, rng, Helper::GenerateRandomInt<Rng, std::size_t>(rng, 20, 30));

	return fs;
}