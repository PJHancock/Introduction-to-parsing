#pragma once
#include <map>
#include "Relation.h"
#include "Scheme.h"

using namespace std;

class Database {
    private:
        map<string, Relation> relations;
    
    public:
        Database() = default;

        map<string, Relation> getRelations() {
            return relations;
        }

        void addRelation(const string& name, const Scheme& scheme) {
            relations[name] = Relation(name, scheme);
        }

        void addFact(const string& name, const vector<string>& values) {
            relations[name].addTuple(Tuple(values));
        }
};