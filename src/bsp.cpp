//
// Created by zeroc00l on 11/13/23.
//
#include "../include/bsp.hpp"

BSP::Node::Node(): leftChild(nullptr), rightChild(nullptr) {

}

BSP::Tree::Tree(RandomNumberGenerator& rngPtr): rng(rngPtr) {

}

std::vector<sf::IntRect> BSP::Tree::GetLeafValues() {
    auto leafList = GetLeafList();
    std::vector<sf::IntRect> rectList;
    for(auto leaf : leafList) {
        rectList.push_back(leaf->rect);
    }
    return rectList;
}

std::vector<BSP::Node*> BSP::Tree::GetLeafList() {
    leafList.clear();
    CreateLeafList(root);
    return leafList;
}

/*
void BSP::Tree::Split(int minWidth, int minHeight, int maxWidth, int maxHeight) {
    Split(root, minWidth, minHeight, maxWidth, maxHeight);
}
*/



void BSP::Tree::CreateRootNode(sf::IntRect rect) {
    root = new BSP::Node;
    root->rect = rect;
}

void BSP::Tree::Split(float minRatio, float maxRatio, int minWidth, int minHeight) {
    Split(root, minRatio, maxRatio, minWidth, minHeight);
}

BSP::Node* BSP::Tree::CreateNode(sf::IntRect rect) {
    Node* node = new BSP::Node;
    node->rect = rect;
    return node;
}

void BSP::Tree::CreateLeafList(BSP::Node* rootPtr) {
    if(!rootPtr) { return; }

    if(!rootPtr->leftChild && !rootPtr->rightChild) {
        leafList.push_back(rootPtr);
    }

    CreateLeafList(rootPtr->leftChild);
    CreateLeafList(rootPtr->rightChild);
}

int BSP::Tree::GetMaxNodeWidth() {
    int maxWidth(0);
    for(auto& leaf : leafList) {
        if(leaf->rect.width > maxWidth) {
            maxWidth = leaf->rect.width;
        }
    }
    return maxWidth;
}

int BSP::Tree::GetMaxNodeHeight() {
    int maxHeight(0);
    for(auto& leaf : leafList) {
        if(leaf->rect.height > maxHeight) {
            maxHeight = leaf->rect.height;
        }
    }
    return maxHeight;
}

void BSP::Tree::Split(BSP::Node* rootPtr, float minRatio, float maxRatio, int minWidth, int minHeight) {
    if(rootPtr->rect.width < (minWidth * 2) || rootPtr->rect.height < (minHeight * 2)) {
        return;
    }

    BSP::SplitDirection splitDirection( BSP::SplitDirection::Horizontal );
    // Splitting horizontally increases this ratio; splitting vertically decreases it.
    float currentRatio = rootPtr->rect.width / rootPtr->rect.height;

    if(currentRatio >= maxRatio) {
        splitDirection = BSP::SplitDirection::Vertical;
    } else {
        splitDirection = BSP::SplitDirection::Horizontal;
    }

    sf::IntRect leftRect, rightRect;
    switch(splitDirection) {
        case BSP::SplitDirection::Horizontal: {
            int height = minHeight + rng.GetRandom(0, rootPtr->rect.height - minHeight);
            leftRect = {
                    rootPtr->rect.left, rootPtr->rect.top, rootPtr->rect.width, height
            };
            rightRect = {
                    rootPtr->rect.left, rootPtr->rect.top + height, rootPtr->rect.width, rootPtr->rect.height - height
            };
            break; }
        case BSP::SplitDirection::Vertical: {
            int width = minWidth + rng.GetRandom(0, rootPtr->rect.width - minWidth);
            leftRect = {
                    rootPtr->rect.left, rootPtr->rect.top, width, rootPtr->rect.height
            };
            rightRect = {
                    rootPtr->rect.left + width, rootPtr->rect.top, rootPtr->rect.width - width, rootPtr->rect.height
            };
            break; }
    }
    rootPtr->leftChild = CreateNode(leftRect);
    rootPtr->rightChild = CreateNode(rightRect);
    Split(rootPtr->leftChild, minRatio, maxRatio, minWidth, minHeight);
    Split(rootPtr->rightChild, minRatio, maxRatio, minWidth, minHeight);
}
/*
void BSP::Tree::Split(BSP::Node* rootPtr, int minWidth, int minHeight, int maxWidth, int maxHeight) {

    BSP::SplitDirection splitDirection( BSP::SplitDirection::Horizontal );
    int probabilityHorizontal = 45;
    int dieRoll = rng.GetRandom(1, 100);

    if(rootPtr->rect.width > maxWidth || rootPtr->rect.height > maxHeight) {
        if (rootPtr->rect.width < rootPtr->rect.height || dieRoll <= probabilityHorizontal) {
            splitDirection = BSP::SplitDirection::Horizontal;
        } else {
            splitDirection = BSP::SplitDirection::Vertical;
        }
    }

    bool canSplit(false);
    if(rootPtr->rect.width > minWidth && rootPtr->rect.height > minHeight) {
        canSplit = true;
    }

    if(canSplit) {
        sf::IntRect leftRect, rightRect;
        if(splitDirection == BSP::SplitDirection::Horizontal) {
            int height = minHeight + rng.GetRandom(0, rootPtr->rect.height - minHeight);
            leftRect = {
                    rootPtr->rect.left, rootPtr->rect.top, rootPtr->rect.width, height
            };
            rightRect = {
                    rootPtr->rect.left, rootPtr->rect.top + height, rootPtr->rect.width, rootPtr->rect.height - height
            };
        } else if(splitDirection == BSP::SplitDirection::Vertical) {
            int width = minWidth + rng.GetRandom(0, rootPtr->rect.width - minWidth);
            leftRect = {
                    rootPtr->rect.left, rootPtr->rect.top, width, rootPtr->rect.height
            };
            rightRect = {
                    rootPtr->rect.left + width, rootPtr->rect.top, rootPtr->rect.width - width, rootPtr->rect.height
            };
        }
        rootPtr->leftChild = CreateNode(leftRect);
        rootPtr->rightChild = CreateNode(rightRect);
        Split(rootPtr->leftChild, minWidth, minHeight, maxWidth, maxHeight);
        Split(rootPtr->rightChild, minWidth, minHeight, maxWidth, maxHeight);
    }
}
*/