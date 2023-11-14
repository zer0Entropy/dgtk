//
// Created by zeroc00l on 11/13/23.
//

#include "location.hpp"
#include "path.hpp"
#include "map.hpp"
#include "log.hpp"

#ifndef DGTKPROJECT_DIJKSTRA_HPP
#define DGTKPROJECT_DIJKSTRA_HPP

namespace Dijkstra {

  struct Node {
      MapLocation location;
      int       distance;
      Node*     previous;
      Node(): distance(-1), previous(nullptr) {}
      Node(const Node& copy) = default;
      ~Node() = default;
  };

  class DistanceMap {
  public:
      DistanceMap(LogSystem* logSystemPtr);
      DistanceMap(const DistanceMap& copy) = default;
      ~DistanceMap() = default;

      Node*                 GetNode(int x, int y);
      void                  Generate(const MapLocation& originLocation, const Map& source);
      Path                  FindPath(const MapLocation& destination);
  private:
      void                  PopulateFrontier();
      void                  PopulateAdjacent();

      LogSystem*            logSystem;
      MapLocation           origin;
      Node*                 currentNode;
      std::deque<Node*>     frontier;
      std::deque<Node*>     adjacent;
      int                   width;
      int                   height;
      Node                  nodeMap[MaxMapHeight][MaxMapWidth];
  };

};

#endif //DGTKPROJECT_DIJKSTRA_HPP
