#pragma once
#include <string>
#include <set>
#include "Scheme.h"
#include "Tuple.h"
#include <unordered_map>

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

  Relation project(vector<int> indices) const {
    vector<string> newNames;
    for (size_t i = 0; i < indices.size(); i++) {
        if (indices[i] < 0 || indices[i] >= static_cast<int>(scheme.size())) {
            throw out_of_range("Invalid projection index");
        }
        newNames.push_back(scheme.at(indices[i]));
    }
    
    Scheme newScheme(newNames);
    Relation result(name, newScheme);

    // Add tuples with projected values
    for (const auto& tuple : tuples) {
        vector<string> newValues;
        for (int index : indices) {
            if (index >= 0 && index < static_cast<int>(tuple.size())) {  // Safety check
                newValues.push_back(tuple.at(index));
            }
        }
        result.addTuple(Tuple(newValues));
    }

    return result;
  }

  Relation rename(vector<int> indicies, vector<string> values) const {
    vector<string> newNames = this->scheme;
    for (size_t i = 0; i < indicies.size(); i++) {
      newNames[indicies[i]] = values[i];
    }
    Scheme newScheme(newNames);
    Relation result(name, newScheme);
    for (const auto& tuple : tuples) {
        result.addTuple(tuple); // Copy all tuples
    }
    return result;
  }

  static bool joinable(const Scheme& leftScheme, const Scheme& rightScheme,
		       const Tuple& leftTuple, const Tuple& rightTuple) {
    // add code to test whether the tuples are joinable
    // loop over leftScheme
    for (unsigned leftIndex = 0; leftIndex < leftScheme.size(); leftIndex++) {
      const string& leftName = leftScheme.at(leftIndex);
      const string& leftValue = leftTuple.at(leftIndex);
      // loop over rightScheme
      for (unsigned rightIndex = 0; rightIndex < rightScheme.size(); rightIndex++) {
        const string& rightName = rightScheme.at(rightIndex);
        const string& rightValue = rightTuple.at(rightIndex);
        if (leftName == rightName && leftValue != rightValue) {
          return false;
        }
      }
    }
    return true;
  }

  static Scheme joinSchemes(const Scheme& leftScheme, const Scheme& rightScheme) {
    Scheme newScheme = leftScheme;
    for (const auto& newAttribute : rightScheme) {
      bool containsAttribute = false;
      for (const auto& existingAttribute : leftScheme) {
        if (existingAttribute == newAttribute) {
          containsAttribute = true;
        }
      }
      if (containsAttribute == false) {
        newScheme.push_back(newAttribute);
      }
    }
    return newScheme;
  }

  static Tuple joinTuples(const Scheme& leftScheme, const Scheme& rightScheme, 
                        const Tuple& leftTuple, const Tuple& rightTuple) {
    Tuple newTuple = leftTuple;  // Start with left tuple

    // Map leftScheme attributes to their values for quick lookup
    unordered_map<string, string> leftAttributeValues;
    for (size_t i = 0; i < leftScheme.size(); i++) {
        leftAttributeValues[leftScheme[i]] = leftTuple[i];
    }

    // Iterate over the right scheme and only add values if they don't conflict
    for (size_t rightIndex = 0; rightIndex < rightScheme.size(); ++rightIndex) {
        const string& rightAttribute = rightScheme[rightIndex];
        const string& rightValue = rightTuple[rightIndex];

        // If the attribute exists in leftScheme, ensure values match
        if (leftAttributeValues.find(rightAttribute) != leftAttributeValues.end()) {
            if (leftAttributeValues[rightAttribute] != rightValue) {
                return Tuple(); // Return an empty tuple (invalid join)
            }
        } else {
            // Otherwise, add the value to the new tuple
            newTuple.push_back(rightValue);
        }
    }

    return newTuple;
  }

  Relation join(Relation right) {
    Relation left = *this;
    Scheme newScheme = left.joinSchemes(left.getScheme(), right.getScheme());
    Relation result(left.getName(), newScheme);
    // add code to complete the join operation
    for (const auto& leftTuple : left.tuples) {
      for (const auto& rightTuple : right.tuples) {
        if (left.joinable(left.scheme, right.scheme, leftTuple, rightTuple)) {
          Tuple newTuple = joinTuples(left.scheme, right.scheme, leftTuple, rightTuple);
          result.addTuple(newTuple);
        }
      }
    }
    return result;
  }
};