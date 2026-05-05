#pragma once
#include <string>
#include <set>
#include "Scheme.h"
#include "Tuple.h"

using namespace std;

class Relation {

 private:

  string name;
  Scheme scheme;
  set<Tuple> tuples;

 public:
  Relation() = default;

  Relation(const string& name, const Scheme& scheme)
    : name(name), scheme(scheme) { }

  string getName() {
    return name;
  }

  Scheme getScheme() {
    return scheme;
  }

  set<Tuple> getTuples() {
    return tuples;
  }

  void addTuple(const Tuple& tuple) {
    tuples.insert(tuple);
  }

  string toString() const {
    stringstream out;
    // add code to print the Tuples, one per line
    for (auto it = tuples.begin(); it != tuples.end(); it++) {
        out << it->toString(scheme) << endl;
    }
    return out.str();
  }

  Relation select(int index, const string& value) const {
    Relation result(name, scheme);
    // add tuples to the result if they meet the condition
    for (auto it = tuples.begin(); it != tuples.end(); it++) {
        if (it->at(index) == value) {
            result.addTuple(*it);
        }
    }
    return result;
  }

  Relation select(int index1, int index2) const {
    Relation result(name, scheme);
    for (const auto& tuple : tuples) {
        if (tuple.at(index1) == tuple.at(index2)) {
            result.addTuple(tuple);
        }
    }
    return result;
  }

  Relation project(vector<int> indicies) const {
    vector<string> newNames;
    for (size_t i = 0; i < indicies.size(); i++) {
        newNames.push_back(scheme.at(indicies[i]));
    }
    Scheme newScheme = Scheme(newNames);
    Relation result(name, newNames);
    // add tuples to the result if they meet the condition
    for (const auto& tuple : tuples) {
        vector<string> newValues;
        for (size_t index : indicies) {
          newValues.push_back(tuple.at(index));
        }
        Tuple newTuple = Tuple(newValues);
        result.addTuple(newTuple);  // Create new tuple with only the projected column
    }
    return result;
  }

  Relation rename(int index, const string& value) const {
    Scheme newScheme = scheme;
    newScheme[index] = value;
    Relation result(name, newScheme);
    for (const auto& tuple : tuples) {
        result.addTuple(tuple); // Copy all tuples
    }
    return result;
  }

};