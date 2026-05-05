#pragma once
#include "Parser.h"
#include "Relation.h"
#include "Database.h"
#include <unordered_set>
#include <unordered_map>

using namespace std;

class Interpreter {
    private:
        DatalogProgram Datalog;
        Database db;

    public:
        // Constructor
        Interpreter(const DatalogProgram& Datalog) : Datalog(Datalog), db() { };

        void interpret() {
            evalSchemes();
            evalFacts();
            vector<Relation> evaluatedQueries = evalQueries();
            outputQueries(evaluatedQueries);
        }

        void outputQueries(vector<Relation> evaluatedQueries) {
            vector<Predicate> queries = Datalog.getQueries();
            for (size_t i = 0; i < evaluatedQueries.size(); i++) {
                stringstream out;
                out << queries[i].toString() << "? ";
                if (evaluatedQueries[i].getTuples().empty()) {
                    out << "No" << endl;
                } else {
                    out << "Yes(" << evaluatedQueries[i].getTuples().size() << ")" << endl;
                }
                bool hasVariable = false;
                for (size_t j = 0; j < queries[i].getParameters().size(); j++) {
                    if (!queries[i].getParameters()[j].isConstant()) {
                        hasVariable = true;
                    }
                }
                if (hasVariable) {
                    set<Tuple> tuples = evaluatedQueries[i].getTuples();
                    for (auto it = tuples.begin(); it != tuples.end(); it++) {
                        out << "  ";
                        out << it->toString(evaluatedQueries[i].getScheme()) << endl;
                    }
                }
                cout << out.str();
            }
        }

        void evalSchemes() {
            for (size_t i = 0; i < Datalog.getSchemes().size(); i++) {
                vector<string> attributes;
                for (size_t j = 0; j < Datalog.getSchemes()[i].getParameters().size(); j++) {
                    attributes.push_back(Datalog.getSchemes()[i].getParameters()[j].toString());
                }
                db.addRelation(Datalog.getSchemes()[i].getName(), attributes);
            }
        }

        void evalFacts() {
            for (size_t i = 0; i < Datalog.getFacts().size(); i++) {
                string name = Datalog.getFacts()[i].getName();
                vector<string> values;
                for (size_t j = 0; j < Datalog.getFacts()[i].getParameters().size(); j++) {
                    values.push_back(Datalog.getFacts()[i].getParameters()[j].toString());
                }
                db.addFact(name, values);
            }
        }

        vector<Relation> evalQueries() {
            vector<Relation> evaluatedQueries;
            for (size_t i = 0; i < Datalog.getQueries().size(); i++) {
                evaluatedQueries.push_back(evalQuery(Datalog.getQueries()[i]));
            }
            return evaluatedQueries;
        }

        Relation evalQuery(Predicate query) {
            map<string, Relation> relations = db.getRelations();
            // Find corresponding relation
            Relation queryRelation = relations[query.getName()];

            vector<Parameter> parameters = query.getParameters();

            // Use one or more select operations to select the tuples from the Relation that match the query
            unordered_map<string, int> variablePositions;
            for (size_t i = 0; i < parameters.size(); i++) {
                Parameter parameter = parameters[i];
                if (parameter.isConstant()) {
                    queryRelation = queryRelation.select(i, parameter.toString());
                } else {
                    string variable = parameter.toString();
                    if (variablePositions.count(variable)) {
                        // If variable appeared before, ensure values match at both positions
                        queryRelation = queryRelation.select(variablePositions[variable], i);
                    } else {
                        // Store first occurrence of the variable
                        variablePositions[variable] = i;
                    }
                    
                    
                    // bool hasMatch = false;
                    // Relation matched_relation(query.getName(), queryRelation.getScheme());

                    // // See if two variable names match in the query
                    // for (size_t j = i + 1; j < parameters.size(); j++) {
                    //     if (variable == parameters[j].toString()) {  // Compare variable names
                    //         set<Tuple> tuples = queryRelation.getTuples();
                    //         for (auto it = tuples.begin(); it != tuples.end(); it++) {
                    //             if (it->at(i) == it->at(j)) {  // Ensure values match in both positions
                    //                 matched_relation.addTuple(*it);
                    //                 hasMatch = true;
                    //             }
                    //         }
                    //     }
                    // }
                    // // If matches were found, update queryRelation
                    // if (hasMatch) {
                    //     queryRelation = matched_relation;
                    // }
                }
            }
            // Keep only the columns from the Relation that correspond to the positions of the variables in the query
            vector<int> queryVariableIndices;
            vector<string> seenVariables;
            unordered_set<string> seenVariablesSet;

            for (size_t i = 0; i < query.getParameters().size(); i++) {
                Parameter param = query.getParameters()[i];
                if (!param.isConstant()) {  // Only consider variables
                    string variable = param.toString();

                    if (seenVariablesSet.insert(variable).second) {
                        // Only add index if variable was not seen before
                        queryVariableIndices.push_back(i);
                        seenVariables.push_back(variable);
                    }
                }   
            }
            queryRelation = queryRelation.project(queryVariableIndices);

            // Rename the scheme of the Relation to the names of the variables found in the query.
            for (size_t i = 0; i < seenVariables.size(); i++) {
                queryRelation = queryRelation.rename(i, seenVariables[i]);
            }

            return queryRelation;
        }

};