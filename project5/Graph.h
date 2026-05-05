#pragma once

#include <map>
#include "Node.h"

using namespace std;

class Graph {

 private:

  map<int, Node> nodes;

 public:

  Graph(int size) {
    for (int nodeID = 0; nodeID < size; nodeID++)
      nodes.emplace(nodeID, Node(nodeID));
  }

  map<int, Node>& getNodes() {
    return nodes;
  }

  void resetVisited() {
    for (auto& pair : nodes) {
      pair.second.setVisited(false);
    }
  }

  string toString() {
    stringstream out;
    for (auto& pair: nodes) {
      int nodeID = pair.first;
      Node& node = pair.second;
      out << "R" << nodeID << ":" << node.toString() << endl;
    }
    return out.str();
  }

  void addEdge(int fromNodeID, int toNodeID) {
    nodes[fromNodeID].addEdge(toNodeID);
  }

};