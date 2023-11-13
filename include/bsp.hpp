//
// Created by zeroc00l on 11/12/23.
//

#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include "log.hpp"
#include "math.hpp"

#ifndef DGTKPROJECT_BSP_HPP
#define DGTKPROJECT_BSP_HPP

namespace BSP {

    enum class SplitDirection {
        Horizontal = 0,
        Vertical
    };

    struct Node {
        sf::IntRect             area;
        Node*                   leftChild;
        Node*                   rightChild;
    };

    class Tree {
    public:
        Tree(RandomNumberGenerator& randomNumberGenerator, LogSystem* logSystemPtr);
        Tree(const Tree& copy) = default;
        ~Tree();

        void                        RemoveSubtree(Node* rootPtr);

        void                        Split(int minWidth, int minHeight, int maxWidth, int maxHeight);

        void                        AddLeaf(sf::IntRect area);

        void                        ConstructListOrder();

        std::vector<sf::IntRect>    GetListOrder();

        void                        ConstructLeafList();

        std::vector<sf::IntRect>    GetLeafList();

        void                        PreorderTraversal();

        void                        InOrderTraversal();
    private:
        Node*                       CreateLeaf(sf::IntRect area);

        void                        Split(Node* rootPtr, int minWidth, int minHeight, int maxWidth, int maxHeight);

        void                        ConstructListOrder(Node* rootPtr);

        void                        ConstructLeafList(Node* rootPtr);

        void                        PreorderTraversal(Node* rootPtr);

        void                        InOrderTraversal(Node* rootPtr);

        RandomNumberGenerator&      rng;
        LogSystem*                  logSystem;
        Node*                       root;
        std::vector<sf::IntRect>    listOrder;
    };

};

#endif //DGTKPROJECT_BSP_HPP
