//
// Created by zeroc00l on 11/13/23.
//

#include "location.hpp"
#include "path.hpp"
#include "log.hpp"

#ifndef DGTKPROJECT_DIJKSTRA_HPP
#define DGTKPROJECT_DIJKSTRA_HPP

class Map;

namespace Dijkstra {

  struct Node {
      MapLocation   location;
      int           distance;
      Node*         previous;
      Node():
        distance(-1), previous(nullptr) {}
      Node(const Node& copy) = default;
      ~Node() = default;
  };

  constexpr int MaxWidth(100);
  constexpr int MaxHeight(100);

  class DistanceMap {

  public:
      DistanceMap();
      DistanceMap(const DistanceMap& copy) = default;
      ~DistanceMap() = default;

      const MapLocation&    GetOrigin() const;
      Node*                 GetNode(int x, int y) const;
      void                  Generate(const MapLocation& originLocation, int mapWidth, int mapHeight);
      Path                  FindPath(const MapLocation& destination);

  protected:
      virtual void          PopulateFrontier();

      MapLocation           origin;
      Node*                 currentNode;
      std::deque<Node*>     frontier;
      std::deque<Node*>     adjacent;
      int                   width;
      int                   height;
      Node                  nodeMap[MaxHeight][MaxWidth];
  };

  constexpr int             FloorWeight{1};
  constexpr int             WallWeight{99};

  class WeightedDistanceMap: public DistanceMap {
  public:
      void                  InitWeightsByWalkability(const Map& map);
      void                  InitWeightsByVisibility(const Map& map);

  private:
      void                  PopulateFrontier() override;
      int                   nodeWeights[MaxHeight][MaxWidth];
  };

};

#endif //DGTKPROJECT_DIJKSTRA_HPP
