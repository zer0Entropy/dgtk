//
// Created by zeroc00l on 11/12/23.
//

#include <vector>
#include "math.hpp"

#ifndef DGTKPROJECT_BSP_HPP
#define DGTKPROJECT_BSP_HPP

namespace BSP {

    constexpr int MinLeafWidth(6);
    constexpr int MinLeafHeight(4);

    struct Node {
        int top;
        int left;
        int width;
        int height;
        int childCount;
        Node* parent;
        Node* leftChild;
        Node* rightChild;
        explicit Node(Node* parentPtr);
        Node(const Node& copyNode) = default;
        ~Node() = default;
    };

    struct Tree {
        Node* root;
        std::vector<Node*> leafList;

        explicit Tree(Node* rootNode);
        Tree(const Tree& copyTree) = default;
        ~Tree() = default;

        Tree GetLeftSubtree() const;
        Tree GetRightSubtree() const;

        int GetNodeCount() const;
        const std::vector<Node*>& GetLeafList();

        void SplitLeaves(RandomNumberGenerator& rng);
    };

};


#endif //DGTKPROJECT_BSP_HPP
