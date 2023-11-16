//
// Created by zeroc00l on 11/13/23.
//

#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include "math.hpp"

#ifndef DGTKPROJECT_BSP_HPP
#define DGTKPROJECT_BSP_HPP

namespace BSP {

    enum class SplitDirection {
        Horizontal = 0,
        Vertical
    };

    struct Node {
        sf::IntRect     rect;
        Node*           leftChild;
        Node*           rightChild;
        Node();
    };

    class Tree {
    public:
        Tree(RandomNumberGenerator& rngPtr);

        std::vector<sf::IntRect>    GetLeafValues();
        std::vector<Node*>          GetLeafList();
        int                         GetLeafCount() const;
        //void                        Split(int minWidth, int minHeight, int maxWidth, int maxHeight);
        void                        Split(float minRatio, float maxRatio, int minWidth, int minHeight);
        void                        CreateRootNode(sf::IntRect rect);
        Node*                       CreateNode(sf::IntRect rect);
    private:
        RandomNumberGenerator&  rng;
        Node*   root;
        std::vector<BSP::Node*> leafList;

        void                CreateLeafList(Node* rootPtr);
        int                 GetLeafCount(Node* rootPtr) const;
        //void                Split(Node* rootPtr, int minWidth, int minHeight, int maxWidth, int maxHeight);
        void                Split(Node* rootPtr, float minRatio, float maxRatio, int minWidth, int minHeight);
    };

};


#endif //DGTKPROJECT_BSP_HPP
