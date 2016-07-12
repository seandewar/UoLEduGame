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


GameFilesystemNode* GameFilesystemNode::AddChildNode(std::unique_ptr<GameFilesystemNode>& node)
{
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
	std::vector<const GameFilesystemNode*> nodesInPathToRoot;

	// currentNode will be null if the last node was the root of the graph
	// (or the root directory)!
	auto currentNode = &node;
	while (currentNode) {
		nodesInPathToRoot.emplace_back(currentNode);
		currentNode = currentNode->GetParent();
	}

	// construct path str
	std::ostringstream oss;
	for (auto it = nodesInPathToRoot.rbegin(); it != nodesInPathToRoot.rend(); ++it) {
		if ((*it)->IsRootDirectoryNode()) {
			oss << '/';
		}
		else {
			oss << (*it)->GetName();

			// don't print a slash at the end of the string
			if (it != nodesInPathToRoot.rend() - 1) {
				oss << '/';
			}
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


GameFilesystemNode* GameFilesystem::SetRootNode(std::unique_ptr<GameFilesystemNode>& node)
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