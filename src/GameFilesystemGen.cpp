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
        "poe",
        "sign",
        "123",
        "wow",
        "roach",
        "fish",
        "alien",
        "aaa",
        "doc",
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


const std::array<std::string, 40> GameFilesystemGen::userNames_ = {
    {
        "gordon",
        "john",
        "james",
        "aaron",
        "myranda",
        "sean",
        "josh",
        "pitcher",
        "dewar",
        "carl",
        "stephen",
        "charlie",
        "fred",
        "alex",
        "hannah",
        "emily",
        "geralt",
        "celeste",
        "anna",
        "leigh",
        "lewis",
        "callum",
        "joel",
        "jacob",
        "jason",
        "sandy",
        "julie",
        "sian",
        "daniel",
        "matthew",
        "parker",
        "sam",
        "jake",
        "daisy",
        "naomi",
        "wendy",
        "david",
        "tux",
        "shepard",
        "tess"
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
            nodeName = GenerateRandomPhraseName(rng);

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


std::size_t GameFilesystemGen::GenRandomFileDirsRecurse(GameFilesystemNode* node, Rng& rng, std::size_t numDirsLeft,
    bool firstDir)
{
    if (!node || !node->IsDirectory()) {
        return numDirsLeft;
    }

    GenerateRandomFiles(*node, rng, Helper::GenerateRandomInt<Rng, std::size_t>(rng, 0, 15));

    while ((firstDir || Helper::GenerateRandomBool(rng, 0.5f)) && numDirsLeft > 0) {
        auto dir = AddNodeWithUniqueName(*node, std::make_unique<GameFilesystemNode>(
            GenerateRandomPhraseName(rng, 1), GameFilesystemNodeType::Directory, GenerateNewId(rng)));
        assert(dir);

        numDirsLeft = GenRandomFileDirsRecurse(dir, rng, --numDirsLeft);
    }

    return numDirsLeft;
}


void GameFilesystemGen::GenerateHomeDir(GameFilesystemNode& parent, Rng& rng)
{
	auto homeNode = parent.AddChildNode(NEW_NODE("home", Directory, rng));

    assert(userNames_.size() > 0);
    int numUsers = Helper::GenerateRandomInt(rng, 1, 6);

    for (int i = 0; i < numUsers; ++i) {
        auto user = homeNode->AddChildNode(NEW_NODE(
            (userNames_[Helper::GenerateRandomInt<Rng, std::size_t>(rng, 0, userNames_.size() - 1)] + std::to_string(i)),
            Directory, rng));
        assert(user);

        assert(GenRandomFileDirsRecurse(user, rng, Helper::GenerateRandomInt<Rng, std::size_t>(rng, 1, 5)) == 0);
    }
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
}


void GameFilesystemGen::GenerateEtcDir(GameFilesystemNode& parent, Rng& rng)
{
    auto etcNode = parent.AddChildNode(NEW_NODE("etc", Directory, rng));

    etcNode->AddChildNode(NEW_NODE("passwd", TextFile, rng));
    etcNode->AddChildNode(NEW_NODE("sudoers", TextFile, rng));
    etcNode->AddChildNode(NEW_NODE("shadow", TextFile, rng));
    etcNode->AddChildNode(NEW_NODE("ssh", TextFile, rng));

    auto initNode = etcNode->AddChildNode(NEW_NODE("init", Directory, rng));
    initNode->AddChildNode(NEW_NODE("init", TextFile, rng));
    initNode->AddChildNode(NEW_NODE("init.d", TextFile, rng));

    auto networkNode = etcNode->AddChildNode(NEW_NODE("network", Directory, rng));
    networkNode->AddChildNode(NEW_NODE("interfaces", TextFile, rng));
}


void GameFilesystemGen::GenerateProcDir(GameFilesystemNode& parent, Rng& rng)
{
    auto procNode = parent.AddChildNode(NEW_NODE("proc", Directory, rng));

    procNode->AddChildNode(NEW_NODE("uptime", TextFile, rng));
    procNode->AddChildNode(NEW_NODE("sys", TextFile, rng));

    int targetNumProcs = Helper::GenerateRandomInt(rng, 1, 15);

    for (int i = 0; i < targetNumProcs; ++i) {
        try {
            procNode->AddChildNode(NEW_NODE(std::to_string(i == 0 ? 1 : Helper::GenerateRandomInt(rng, 2, 2000)),
                TextFile, rng));
        }
        catch (GameFilesystemException& e) {
            if (e.GetType() != GameFilesystemExceptionType::NameIsTaken) {
                throw;
            }
        }
    }
}


void GameFilesystemGen::GenerateLibDir(GameFilesystemNode& parent, Rng& rng)
{
    auto libNode = parent.AddChildNode(NEW_NODE("lib", Directory, rng));

    libNode->AddChildNode(NEW_NODE("chroot-setup.sh", ShellScriptFile, rng));

    auto initNode = libNode->AddChildNode(NEW_NODE("init", Directory, rng));
    initNode->AddChildNode(NEW_NODE("fstab", TextFile, rng));
    initNode->AddChildNode(NEW_NODE("vars.sh", TextFile, rng));
    initNode->AddChildNode(NEW_NODE("init-d-script", ExecutableFile, rng));
    initNode->AddChildNode(NEW_NODE("upstart-job", ExecutableFile, rng));
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
            AddNodeWithUniqueName(*targetNode, NEW_NODE(GenerateRandomHexName(rng, 5) + ".log", TextFile, rng));
        }

        targetNode = AddNodeWithUniqueName(*tmpNode, NEW_NODE(GenerateRandomHexName(rng, 8), Directory, rng));
    } while (++i <= tmpSubdirs); // <= because we do 1 extra iteration to populate the tmp dir before the subdirs
}


void GameFilesystemGen::GenerateMntDir(GameFilesystemNode& parent, Rng& rng)
{
    auto mntNode = parent.AddChildNode(NEW_NODE("mnt", Directory, rng));

    int numMnts = Helper::GenerateRandomInt(rng, 0, 3);

    for (int i = 0; i < numMnts; ++i) {
        auto mnt = AddNodeWithUniqueName(*mntNode, NEW_NODE(GenerateRandomPhraseName(rng, 1), Directory, rng));
        assert(mnt);

        assert(GenRandomFileDirsRecurse(mnt, rng, Helper::GenerateRandomInt<Rng, std::size_t>(rng, 1, 5)) == 0);
    }
}


void GameFilesystemGen::GenerateMediaDir(GameFilesystemNode& parent, Rng& rng)
{
    auto mediaNode = parent.AddChildNode(NEW_NODE("media", Directory, rng));

    if (Helper::GenerateRandomBool(rng, 0.5f)) {
        auto media = AddNodeWithUniqueName(*mediaNode, NEW_NODE("cdrom", Directory, rng));
        assert(media);

        assert(GenRandomFileDirsRecurse(media, rng, Helper::GenerateRandomInt<Rng, std::size_t>(rng, 1, 5)) == 0);
    }
}


std::unique_ptr<GameFilesystem> GameFilesystemGen::GenerateNewFilesystem()
{
	Rng rng(seed_);
	auto fs = std::make_unique<GameFilesystem>();

	// create root dir
	auto rootNode = fs->SetRootNode(NEW_NODE("root", RootDirectory, rng));
    assert(rootNode);

	// populate root dir
    GenerateBinDir(*rootNode, rng);
    GenerateDevDir(*rootNode, rng);
    GenerateEtcDir(*rootNode, rng);
    GenerateLibDir(*rootNode, rng);
	GenerateHomeDir(*rootNode, rng);
    GenerateMntDir(*rootNode, rng);
    GenerateMediaDir(*rootNode, rng);
    GenerateProcDir(*rootNode, rng);
    GenerateTmpDir(*rootNode, rng);
    GenerateUsrDir(*rootNode, rng);
    GenerateSbinDir(*rootNode, rng);

	return fs;
}