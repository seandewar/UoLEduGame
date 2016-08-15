#include "GameFilesystem.h"

#include <sstream>
#include <cassert>


GameFilesystemNode::GameFilesystemNode(const std::string& name, GameFilesystemNodeType type, RngInt randomId) :
type_(type),
parentNode_(nullptr),
randomId_(randomId)
{
	SetName(name);
}


GameFilesystemNode::~GameFilesystemNode()
{
}


GameFilesystemNode* GameFilesystemNode::AddChildNode(std::unique_ptr<GameFilesystemNode> node)
{
    if (!IsDirectory()) {
        throw GameFilesystemException("A node that isn't a directory cannot have children.");
    }

	auto nodePtr = node.get();

	if (node) {
        if (node->IsRootDirectoryNode()) {
            throw GameFilesystemException("A node of type RootDirectory cannot be a child of any other node!");
        }

        // check if node with this name doesn't already exist as a child of this node
        for (const auto& child : childNodes_) {
            assert(child);
            if (node->GetName() == child->GetName()) {
                throw GameFilesystemException("A child node with the same name already exists!",
                    GameFilesystemExceptionType::NameIsTaken);
            }
        }

		node->parentNode_ = this;
		childNodes_.emplace_back(std::move(node));

		printf("Added: %s (%u)\n", GameFilesystem::GetNodePathString(*nodePtr).c_str(), nodePtr->GetRandomIdentifier());
	}

	return nodePtr;
}


void GameFilesystemNode::SetName(const std::string& name)
{
	if (name.size() == 0) {
		throw GameFilesystemException("Node names cannot be empty.",
            GameFilesystemExceptionType::NameIsEmpty);
	}
	else if (name.size() > 255) {
		throw GameFilesystemException("Node names may not be more than 255 characters.",
            GameFilesystemExceptionType::NameIsTooBig);
	}

	if (name.find_first_of("/\0") != std::string::npos) {
		throw GameFilesystemException("Node names cannot contain '/' or 'NUL' characters.",
            GameFilesystemExceptionType::NameHasInvalidCharacters);
	}

	name_ = name;
}


std::string GameFilesystem::GetNodePathString(const GameFilesystemNode& node)
{
	// traverse through nodes to root directory OR root of graph (if no root directory)
    std::vector<std::string> nodeNamesInPathToRoot;

	// currentNode will be null if the last node was the root of the graph
	// (or the root directory)!
	auto currentNode = &node;
	while (currentNode) {
        bool isRootNode = currentNode->IsRootDirectoryNode();

        nodeNamesInPathToRoot.emplace_back(isRootNode ? "/" : currentNode->GetName());
        currentNode = isRootNode ? nullptr : currentNode->GetParent();
	}

	// construct path str
	std::ostringstream oss;

    for (auto it = nodeNamesInPathToRoot.rbegin(); it != nodeNamesInPathToRoot.rend(); ++it) {
        oss << *it;

        // don't print a slash at the beginning OR end of the string
        // (beginning of string will already have a slash for root)
        if (it != nodeNamesInPathToRoot.rbegin() &&
            it != nodeNamesInPathToRoot.rend() - 1) {
            oss << '/';
        }
    }

	return oss.str();
}


GameFilesystem::GameFilesystem()
{
}


GameFilesystem::~GameFilesystem()
{
}


GameFilesystemNode* GameFilesystem::GetNodeFromPathString(const std::string& path)
{
    if (!GetRootNode()) {
        return nullptr;
    }

    std::size_t pathStrPos = 0;
    std::string pathNextName;
    GameFilesystemNode* pathCurrentNode = nullptr;
    int pathDepth = -1;

    do {
        bool isLastChar = pathStrPos == path.size() - 1;
        char c = path[pathStrPos];

        // if we find another slash or we're at the end of the string...
        if (c == '/' || isLastChar) {
            if (c == '/' && !isLastChar && path[pathStrPos + 1] == '/') {
                // POSIX paths are allowed to have multiple slashes representing a single slash.
                continue;
            }
            else if (c != '/') {
                // if this is not a slash /, then it's the last character, which means
                // the slash was probably omitted & this is the last char of the next node's
                // name.

                assert(isLastChar);
                pathNextName += c;
            }

            if (pathDepth < 0) {
                // this is the root node slash at the beginning of the path!

                if (!rootNode_) {
                    // we don't have any root node assigned - fail
                    pathCurrentNode = nullptr;
                    break;
                }
                else {
                    pathCurrentNode = rootNode_.get();
                    pathDepth = 0;
                    continue;
                }
            }
            else {
                // search for a node in pathCurrentNode called pathNextName
                auto pathNextNode = pathCurrentNode->GetChildNode(pathNextName);

                if (!pathNextNode) {
                    // no child with that name found
                    pathCurrentNode = nullptr;
                    break;
                }
                else {
                    // we have a valid node!
                    // now make sure that the node is a directory node if
                    // the current path char was a dir slash!

                    if (c == '/' && !pathNextNode->IsDirectory()) {
                        // dir slash used on file node - fail
                        pathCurrentNode = nullptr;
                        break;
                    }
                    else {
                        pathCurrentNode = pathNextNode;
                        pathNextName.clear();
                        ++pathDepth;
                        continue;
                    }
                }
            }
        }
        else {
            if (pathDepth < 0) {
                // path does not contain root slash - fail
                pathCurrentNode = nullptr;
                break;
            }
            else {
                pathNextName += c;
                continue;
            }
        }
    } while (pathStrPos++ < path.size());

    return pathCurrentNode;
}


GameFilesystemNode* GameFilesystem::SetRootNode(std::unique_ptr<GameFilesystemNode> node)
{
	if (!node->IsRootDirectoryNode()) {
		throw GameFilesystemException("The root node of a GameFilesystem should be of type RootDirectory!");
	}

	auto nodePtr = node.get();

	if (node) {
		node->parentNode_ = nullptr;
		rootNode_ = std::move(node);
	}

	return nodePtr;
}