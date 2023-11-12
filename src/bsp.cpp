//
// Created by zeroc00l on 11/12/23.
//
#include "../include/bsp.hpp"

BSP::Node::Node(Node* parentPtr): parent(parentPtr), childCount(0), leftChild(nullptr), rightChild(nullptr) {}

BSP::Tree::Tree(BSP::Node* rootNode): root(rootNode) {}

BSP::Tree BSP::Tree::GetLeftSubtree() const {
    if(!root) { return Tree(nullptr); }
    return Tree(root->leftChild);
}
BSP::Tree BSP::Tree::GetRightSubtree() const {
    if(!root) { return Tree(nullptr); }
    return Tree(root->rightChild);
}
int BSP::Tree::GetNodeCount() const {
    int nodeCount(0);
    if(!root) { return nodeCount; }
    else {
        ++nodeCount;
        if(root->leftChild) {
            nodeCount += GetLeftSubtree().GetNodeCount();
        }
        if(root->rightChild) {
            nodeCount += GetRightSubtree().GetNodeCount();
        }
    }
    return nodeCount;
}

const std::vector<BSP::Node*>& BSP::Tree::GetLeafList() {
    leafList.clear();
    int nodeCount = GetNodeCount();

    if(nodeCount == 0) { return leafList; }
    if(nodeCount == 1) {
        leafList.push_back(root);
    } else if(nodeCount == 2) {
        leafList.push_back(root->leftChild);
        leafList.push_back(root->rightChild);
    } else {
        auto leftList = GetLeftSubtree().GetLeafList();
        auto rightList = GetRightSubtree().GetLeafList();
        for(auto leftNode : leftList) {
            leafList.push_back(leftNode);
        }
        for(auto rightNode : rightList) {
            leafList.push_back(rightNode);
        }
    }

    return leafList;
}

void BSP::Tree::SplitLeaves(RandomNumberGenerator& rng) {
    auto leafList = GetLeafList();

    int minTop;
    int maxTop;
    int minLeft, maxLeft;
    int minWidth, maxWidth;
    int minHeight, maxHeight;

    for(Node* leaf : leafList) {
        leaf->leftChild = new Node(leaf);
        leaf->rightChild = new Node(leaf);
        leaf->childCount = 2;
        // TOP must be:     greater than leaf->top, no greater than three-quarters (leaf->top + leaf->height)
        minTop = leaf->top + 1;
        maxTop = 3 * (leaf->top + leaf->height) / 4;
        while(maxTop <= minTop) {
            if(minTop >= BSP::MinLeafHeight - 2) {
                minTop -= 2;
            }
            maxTop += 2;
        }
        // LEFT must be:    greater than leaf->left, no greater than three-quarters (leaf->left + leaf->width)
        minLeft = leaf->left + 1;
        maxLeft = 3 * (leaf->left + leaf->width) / 4;
        while(maxLeft <= minLeft) {
            if(minLeft >= BSP::MinLeafWidth - 2) {
                minLeft -= 2;
            }
            maxLeft += 2;
        }
        // WIDTH must be:   greater than (leaf->width / 4), no greater than 3 * (leaf->width / 4)
        minWidth = leaf->width / 4;
        maxWidth = 3 * (leaf->width / 4);
        while(maxWidth <= minWidth) {
            if(minWidth >= BSP::MinLeafWidth - 1) {
                --minWidth;
            }
            ++maxWidth;
        }
        // HEIGHT must be:  greater than (leaf->height / 3), no greater than 2 * (leaf->height / 3)
        minHeight = leaf->height / 3;
        maxHeight = 2 * (leaf->height) / 3;
        while(maxHeight <= minHeight) {
            if(minHeight >= BSP::MinLeafHeight - 1) {
                --minHeight;
            }
            ++maxHeight;
        }

        bool retry(false);
        do {
            // Let the random generation commence!
            leaf->leftChild->top = rng.GetRandom(minTop, maxTop);
            leaf->leftChild->left = rng.GetRandom(minLeft, maxLeft);
            leaf->leftChild->width = rng.GetRandom(minWidth, maxWidth);
            leaf->leftChild->height = rng.GetRandom(minHeight, maxHeight);

            bool verticalSplit(false);
            int coinFlip = rng.GetRandom(0, 1);
            if(coinFlip) { verticalSplit = true; }

            if(verticalSplit) {
                leaf->rightChild->top = leaf->leftChild->top;
                leaf->rightChild->left = leaf->leftChild->left + leaf->leftChild->width + 1;
                leaf->rightChild->width = leaf->width - leaf->leftChild->width - 1;
                leaf->rightChild->height = leaf->leftChild->height;
            } else {
                leaf->rightChild->top = leaf->leftChild->top + leaf->leftChild->height;
                leaf->rightChild->left = leaf->leftChild->left;
                leaf->rightChild->width = leaf->leftChild->width;
                leaf->rightChild->height = leaf->height - leaf->leftChild->height;
            }

            if(leaf->leftChild->width < BSP::MinLeafWidth)          { retry = true; }
            else if(leaf->leftChild->height < BSP::MinLeafHeight)   { retry = true; }
            else if(leaf->rightChild->width < BSP::MinLeafWidth)    { retry = true; }
            else if(leaf->rightChild->height < BSP::MinLeafHeight)  { retry = true;}
            else                                                    { retry = false; }
        } while(retry);

    }
}
