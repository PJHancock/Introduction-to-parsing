#pragma once
#include <vector>
#include <sstream>
#include "Scheme.h"
#include <iostream>
using namespace std;

class Tuple : public vector<string> {
    private:
        vector<string> values;
    
    public:
        Tuple() = default;
        
        Tuple(vector<string> values) : vector<string>(values) { };

        vector<string> getTuples() {
            return values;
        }

        string toString(const Scheme& scheme) const {
            const Tuple& tuple = *this;
            stringstream out;

            for (size_t i = 0; i < tuple.size(); i++) {
                out << scheme.at(i) << "=" << tuple.at(i);
                if (i + 1 != tuple.size()) {
                    out << ", ";
                }
            }
            return out.str();
         }
};