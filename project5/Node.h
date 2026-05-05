#pragma once
#include <set>
#include <sstream>

using namespace std;

class Node {

 private:

  set<int> adjacentNodeIDs;
  bool visited;
  int ID;

 public:

  Node() : visited(false), ID(-1) {}
  Node(int ID) : visited(false), ID(ID) {}

  void setVisited(bool visitSet) {
    visited = visitSet;
  }

  int getID() {
    return ID;
  }

  bool getVisited() {
    return visited;
  }

  set<int>& getAdjacentNodeIDs() {
    return adjacentNodeIDs;
  }

  void addEdge(int adjacentNodeID) {
    adjacentNodeIDs.insert(adjacentNodeID);
  }

  string toString() {
    stringstream out;
    for (int edge : adjacentNodeIDs) {
        out << "R" << edge << ",";
    }
    string result = out.str();
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
  }

};
