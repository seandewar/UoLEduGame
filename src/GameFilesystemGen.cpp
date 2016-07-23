#include "GameFilesystemGen.h"

#include <cassert>
#include <sstream>


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


GameFilesystemNode* GameFilesystemGen::AddNodeWithUniqueName(GameFilesystemNode& parent,
    std::unique_ptr<GameFilesystemNode>& node)
{
    // attempt to add the file node with this name.
    // if the name already exists, try with a new unique name by appending
    // a number at the end of it (before the extension)
    std::size_t renameTryCount = 0;
    while (true) {
        try {
            auto addedNode = parent.AddChildNode(node);
            return addedNode;
        }
        catch (const GameFilesystemException& e) {
            if (e.GetType() != GameFilesystemExceptionType::NameIsTaken) {
                // re-throw if it's not a NameIsTaken exception type
                throw;
            }
        }

        ++renameTryCount;

        std::ostringstream oss;
        oss << '(' << renameTryCount << ')' << node->GetName();
        node->SetName(oss.str());
    }
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

        AddNodeWithUniqueName(parent, std::make_unique<GameFilesystemNode>(
            nodeName + nodeNameExt, nodeType, GenerateNewId(rng)));
    }
}


void GameFilesystemGen::GenerateHomeDir(GameFilesystemNode& parent, Rng& rng)
{
	auto homeNode = parent.AddChildNode(NEW_NODE("home", Directory, rng));

	// TODO: Generate users
}


void GameFilesystemGen::GenerateDevDir(GameFilesystemNode& parent, Rng& rng)
{
    auto devNode = parent.AddChildNode(NEW_NODE("dev", Directory, rng));

    devNode->AddChildNode(NEW_NODE("null", NullDevice, rng));
    devNode->AddChildNode(NEW_NODE("zero", ZeroDevice, rng));
    devNode->AddChildNode(NEW_NODE("full", FullDevice, rng));
    devNode->AddChildNode(NEW_NODE("random", RandomDevice, rng));
    devNode->AddChildNode(NEW_NODE("urandom", RandomDevice, rng));
    devNode->AddChildNode(NEW_NODE("console", Device, rng));

    devNode->AddChildNode(NEW_NODE("tty", Device, rng));

    if (Helper::GenerateRandomBool(rng, 0.5)) {
        devNode->AddChildNode(NEW_NODE("tty0", Device, rng));

        if (Helper::GenerateRandomBool(rng, 0.5)) {
            devNode->AddChildNode(NEW_NODE("tty1", Device, rng));
        }
    }

    devNode->AddChildNode(NEW_NODE("ttyS0", Device, rng));

    if (Helper::GenerateRandomBool(rng, 0.5)) {
        devNode->AddChildNode(NEW_NODE("ttyS1", Device, rng));
    }

    devNode->AddChildNode(NEW_NODE("sda", Device, rng));
    devNode->AddChildNode(NEW_NODE("sda1", Device, rng));

    if (Helper::GenerateRandomBool(rng, 0.5)) {
        devNode->AddChildNode(NEW_NODE("sda2", Device, rng));

        if (Helper::GenerateRandomBool(rng, 0.5)) {
            devNode->AddChildNode(NEW_NODE("sda3", Device, rng));
        }
    }
}


void GameFilesystemGen::GenerateBinDir(GameFilesystemNode& parent, Rng& rng)
{
    auto binNode = parent.AddChildNode(NEW_NODE("bin", Directory, rng));

    binNode->AddChildNode(NEW_NODE("cat", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("chmod", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("chown", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("chgrp", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("cksum", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("cmp", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("cp", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("dd", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("df", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("fuser", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("ln", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("ls", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("mkdir", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("mount", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("mv", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("pwd", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("rm", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("rmdir", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("split", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("touch", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("kill", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("ps", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("su", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("uname", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("ed", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("less", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("more", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("sed", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("netcat", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("netstat", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("ping", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("grep", ExecutableFile, rng));
}


void GameFilesystemGen::GenerateSbinDir(GameFilesystemNode& parent, Rng& rng)
{
    auto sbinNode = parent.AddChildNode(NEW_NODE("sbin", Directory, rng));

    sbinNode->AddChildNode(NEW_NODE("fsck", ExecutableFile, rng));
    sbinNode->AddChildNode(NEW_NODE("ifconfig", ExecutableFile, rng));
    sbinNode->AddChildNode(NEW_NODE("route", ExecutableFile, rng));
}


void GameFilesystemGen::GenerateUsrDir(GameFilesystemNode& parent, Rng& rng)
{
    auto usrNode = parent.AddChildNode(NEW_NODE("usr", Directory, rng));

    // TODO: Generate some random files in these directories

    // /usr/bin
    auto binNode = usrNode->AddChildNode(NEW_NODE("bin", Directory, rng));
    binNode->AddChildNode(NEW_NODE("file", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("split", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("tee", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("killall", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("nice", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("pgrep", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("pkill", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("pstree", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("time", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("top", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("clear", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("env", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("logname", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("mesg", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("passwd", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("sudo", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("uptime", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("tput", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("w", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("wall", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("who", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("whoami", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("write", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("awk", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("basename", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("comm", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("csplit", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("cut", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("diff", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("dirname", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("ex", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("fmt", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("fold", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("head", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("iconv", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("join", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("nl", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("paste", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("printf", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("sort", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("strings", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("tail", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("tr", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("uniq", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("vi", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("wc", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("xargs", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("yes", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("dig", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("host", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("nslookup", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("rdate", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("rlogin", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("ssh", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("find", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("locate", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("whatis", ExecutableFile, rng));
    binNode->AddChildNode(NEW_NODE("whereis", ExecutableFile, rng));

    // /usr/sbin
    auto sbinNode = usrNode->AddChildNode(NEW_NODE("sbin", Directory, rng));
    sbinNode->AddChildNode(NEW_NODE("chroot", ExecutableFile, rng));

    // /usr/lib
    auto libNode = usrNode->AddChildNode(NEW_NODE("lib", Directory, rng));
    libNode->AddChildNode(NEW_NODE("sudo", LibraryFile, rng));

    // /usr/share
    usrNode->AddChildNode(NEW_NODE("share", Directory, rng));

    // /usr/include
    usrNode->AddChildNode(NEW_NODE("include", Directory, rng));
}


void GameFilesystemGen::GenerateEtcDir(GameFilesystemNode& parent, Rng& rng)
{
    auto etcNode = parent.AddChildNode(NEW_NODE("etc", Directory, rng));

    etcNode->AddChildNode(NEW_NODE("passwd", TextFile, rng));
    etcNode->AddChildNode(NEW_NODE("sudoers", TextFile, rng));
    etcNode->AddChildNode(NEW_NODE("shadow", TextFile, rng));
    etcNode->AddChildNode(NEW_NODE("ssh", TextFile, rng));

    auto networkNode = etcNode->AddChildNode(NEW_NODE("network", Directory, rng));
    networkNode->AddChildNode(NEW_NODE("interfaces", TextFile, rng));
}


void GameFilesystemGen::GenerateProcDir(GameFilesystemNode& parent, Rng& rng)
{
    auto procNode = parent.AddChildNode(NEW_NODE("proc", Directory, rng));

    procNode->AddChildNode(NEW_NODE("uptime", TextFile, rng));
}


void GameFilesystemGen::GenerateTmpDir(GameFilesystemNode& parent, Rng& rng)
{
    auto tmpNode = parent.AddChildNode(NEW_NODE("tmp", Directory, rng));

    int tmpSubdirs = Helper::GenerateRandomInt(rng, 0, 5);
    GameFilesystemNode* targetNode = tmpNode;
    int i = 0;
    do {
        int tmpFiles = Helper::GenerateRandomInt(rng, 0, 8);
        for (int i = 0; i < tmpFiles; ++i) {
            targetNode->AddChildNode(NEW_NODE(GenerateRandomHexName(rng, 5) + ".log", TextFile, rng));
        }

        targetNode = AddNodeWithUniqueName(*tmpNode, NEW_NODE(GenerateRandomHexName(rng, 8), Directory, rng));
    } while (++i <= tmpSubdirs); // <= because we do 1 extra iteration to populate the tmp dir before the subdirs
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
    GenerateBinDir(*rootNode, rng);
    GenerateDevDir(*rootNode, rng);
    GenerateEtcDir(*rootNode, rng);
	rootNode->AddChildNode(NEW_NODE("lib", Directory, rng));
	GenerateHomeDir(*rootNode, rng);
	rootNode->AddChildNode(NEW_NODE("mnt", Directory, rng));
    GenerateProcDir(*rootNode, rng);
    GenerateTmpDir(*rootNode, rng);
    GenerateUsrDir(*rootNode, rng);
    GenerateSbinDir(*rootNode, rng);

	return fs;
}