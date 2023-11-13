//
// Created by zeroc00l on 11/12/23.
//
#include "../include/bsp.hpp"

BSP::Tree::Tree(RandomNumberGenerator& randomNumberGenerator, LogSystem* logSystemPtr):
    rng(randomNumberGenerator), logSystem(logSystemPtr), root(nullptr) {

}

BSP::Tree::~Tree() {
    RemoveSubtree(root);
}

void BSP::Tree::RemoveSubtree(BSP::Node* rootPtr) {
    if(rootPtr) {
        if(rootPtr->leftChild) {
            RemoveSubtree(rootPtr->leftChild);
        }
        if(rootPtr->rightChild) {
            RemoveSubtree(rootPtr->rightChild);
        }
        delete rootPtr;
    }
}

void BSP::Tree::Split(int minWidth, int minHeight, int maxWidth, int maxHeight) {
    Split(root, minWidth, minHeight, maxWidth, maxHeight);
}

void BSP::Tree::AddLeaf(sf::IntRect area) {
    root = CreateLeaf(area);
}

void BSP::Tree::ConstructListOrder() {
    listOrder.clear();
    ConstructListOrder(root);
}

std::vector<sf::IntRect> BSP::Tree::GetListOrder() {
    return std::vector<sf::IntRect>();
}

void BSP::Tree::ConstructLeafList() {
    ConstructLeafList(root);
}

std::vector<sf::IntRect> BSP::Tree::GetLeafList() {
    ConstructListOrder();
    return listOrder;
}

void BSP::Tree::PreorderTraversal() {
    PreorderTraversal(root);
}

void BSP::Tree::InOrderTraversal() {
    InOrderTraversal(root);
}

BSP::Node* BSP::Tree::CreateLeaf(sf::IntRect area) {
    BSP::Node* leaf = new Node();
    leaf->area = area;
    leaf->leftChild = leaf->rightChild = nullptr;
    return leaf;
}

void BSP::Tree::Split(BSP::Node* rootPtr, int minWidth, int minHeight, int maxWidth, int maxHeight) {
    BSP::SplitDirection splitDirection(BSP::SplitDirection::Vertical);

    // PRE-PROCESSING:  Determine areas eligible for splitting and SplitDirection
    if(rootPtr->area.width > maxWidth || rootPtr->area.height > maxHeight) {
        if(rootPtr->area.width != rootPtr->area.height) {
            if(rootPtr->area.width < rootPtr->area.height) {
                splitDirection = BSP::SplitDirection::Horizontal;
            } else {
                int horizontalProbability = 55;
                int diceRoll = rng.GetRandom(1, 100);
                if(diceRoll <= horizontalProbability) {
                    splitDirection = BSP::SplitDirection::Horizontal;
                } else {
                    splitDirection = BSP::SplitDirection::Vertical;
                }
            }
        }
    }

    bool canSplit(false);
    if(rootPtr->area.width > minWidth && rootPtr->area.height > minHeight) {
        canSplit = true;
    }

    if(canSplit) {
        sf::IntRect leftArea, rightArea;
        if(splitDirection == BSP::SplitDirection::Horizontal) {
            int height = minHeight + rng.GetRandom(0, rootPtr->area.height - minHeight);
            leftArea = {
                    rootPtr->area.left, rootPtr->area.top, rootPtr->area.width, height
            };
            rightArea = {
                    rootPtr->area.left, rootPtr->area.top + height, rootPtr->area.width, rootPtr->area.height - height
            };
        } // HORIZONTAL SPLIT
        else if(splitDirection == BSP::SplitDirection::Vertical) {
            int width = minWidth + rng.GetRandom(0, rootPtr->area.width - minWidth);
            leftArea = {
                    rootPtr->area.left, rootPtr->area.top, width, rootPtr->area.height
            };
            rightArea = {
                    rootPtr->area.left + width, rootPtr->area.top, rootPtr->area.width - width, rootPtr->area.height
            };
        } // VERTICAL SPLIT
        rootPtr->leftChild = CreateLeaf(leftArea);
        rootPtr->rightChild = CreateLeaf(rightArea);
        Split(rootPtr->leftChild, minWidth, minHeight, maxWidth, maxHeight);
        Split(rootPtr->rightChild, minWidth, minHeight, maxWidth, maxHeight);
    } // CAN_SPLIT
}

void BSP::Tree::ConstructListOrder(BSP::Node* rootPtr) {
    if(!rootPtr) { return; }

    listOrder.push_back(rootPtr->area);
    ConstructListOrder(rootPtr->leftChild);
    ConstructListOrder(rootPtr->rightChild);
}

void BSP::Tree::ConstructLeafList(BSP::Node* rootPtr) {
    if(!rootPtr) { return; }

    if(!rootPtr->leftChild && !rootPtr->rightChild) {
        listOrder.push_back(rootPtr->area);
    }

    ConstructLeafList(rootPtr->leftChild);
    ConstructLeafList(rootPtr->rightChild);
}

void BSP::Tree::PreorderTraversal(BSP::Node* rootPtr) {
    if(!rootPtr) { return; }

    std::string message("AREA{ (");
    message.append(std::to_string(rootPtr->area.left));
    message.append(", ");
    message.append(std::to_string(rootPtr->area.top));
    message.append("), width=");
    message.append(std::to_string(rootPtr->area.width));
    message.append(", height=");
    message.append(std::to_string(rootPtr->area.height));
    message.append(" }");
    logSystem->PublishMessage(message);

    PreorderTraversal(rootPtr->leftChild);
    PreorderTraversal(rootPtr->rightChild);
}

void BSP::Tree::InOrderTraversal(BSP::Node* rootPtr) {
    if(!rootPtr) { return; }
    if(rootPtr->leftChild) {
        InOrderTraversal(rootPtr->leftChild);

        std::string message("AREA{ (");
        message.append(std::to_string(rootPtr->area.left));
        message.append(", ");
        message.append(std::to_string(rootPtr->area.top));
        message.append("), width=");
        message.append(std::to_string(rootPtr->area.width));
        message.append(", height=");
        message.append(std::to_string(rootPtr->area.height));
        message.append(" }");
        logSystem->PublishMessage(message);

        InOrderTraversal(rootPtr->rightChild);
    }
}
