#pragma once
#include "Parser.h"
#include "Relation.h"
#include "Database.h"
#include "Graph.h"
#include <stack>
#include <unordered_set>
#include <unordered_map>

using namespace std;

class Interpreter {
    private:
        DatalogProgram Datalog;
        Database db;

    public:
        // Constructor
        Interpreter(DatalogProgram Datalog) : Datalog(Datalog), db() { };

        static Graph makeGraph(const vector<Rule> rules, bool reverse = false) {
            Graph graph(rules.size());
            // add code to add edges to the graph for the rule dependencies

            // loop over the rules
            for (size_t i = 0; i < rules.size(); i++) {
                Rule fromRule = rules[i];
                // cout << "Processing Rule " << i << " (Head: " << fromRule.getHeadPredicate().toString() << ")\n";
                // loop over predicates in rule body
                for (Predicate& bodyPredicate : fromRule.getBodyPredicates()) {
                    // cout << "  Checking Body Predicate: " << bodyPredicate.toString() << "\n";
                    // loop over rules again
                    for (size_t k = 0; k < rules.size(); k++) {
                        Rule toRule = rules[k];
                        // cout << "    Comparing with Rule " << k << " (Head: " << toRule.getHeadPredicate().toString() << ")\n";
                        // see if there is a dependency
                        if (bodyPredicate.getName() == toRule.getHeadPredicate().getName()) {
                            if (reverse) {
                                graph.addEdge(k, i);
                                // cout << "    [ADD EDGE] Reverse Mode: " << k << " -> " << i << "\n";
                            } else {
                                graph.addEdge(i, k);
                                // cout << "    [ADD EDGE] Normal Mode: " << i << " -> " << k << "\n";
                            }
                        }
                    }
                }
            }
            return graph;
        }

        static void DFS(Graph& graph, Node& node, stack<int>& postOrder) {
            node.setVisited(true);
            // cout << "Visiting node: " << node.getID() << " (Visited: " << node.getVisited() << ")" << endl;
            for (int NodeID : node.getAdjacentNodeIDs()) {
                Node& adjacent = graph.getNodes().at(NodeID);
                // cout << "Checking adjacent node: " << adjacent.getID() << " (Visited: " << adjacent.getVisited() << ")" << endl;
                if (!adjacent.getVisited()) {
                    DFS(graph, adjacent, postOrder);
                }
            }
            // cout << "Pushing node " << node.getID() << " to postOrder stack" << endl;
            postOrder.push(node.getID());
        }

        static stack<int> DFS_Forest(Graph& reverseGraph) {
            stack<int> postOrder;
            // Set all as not visited
            reverseGraph.resetVisited();
            // Go through and visit each node
            for (auto& pair : reverseGraph.getNodes()) {
                Node& node = pair.second;
                if (!node.getVisited()) {
                    DFS(reverseGraph, node, postOrder);
                }
            }

            return postOrder;
        }

        static map<int, vector<int>> findSCCs(stack<int>& postOrder, Graph& dependencyGraph) {
            map<int, vector<int>> SCCs;
            dependencyGraph.resetVisited();
            int index = 1;
            while (!postOrder.empty()) {
                int nodeID = postOrder.top();
                postOrder.pop();
                Node& node = dependencyGraph.getNodes().at(nodeID);
                if (!node.getVisited()) {
                    // cout << "Starting DFS for SCC " << index << " from node: " << nodeID << endl;
                    stack<int> SCC;
                    DFS(dependencyGraph, node, SCC);
                    set<int> SCCNodesSorted;
                    while (!SCC.empty()) {
                        int sccNodeID = SCC.top();
                        SCCNodesSorted.insert(sccNodeID);
                        // cout << " - Adding node to SCC " << index << ": " << sccNodeID << endl;
                        SCC.pop();
                    }
                    vector<int> SCCNodes;
                    for (int i : SCCNodesSorted) {
                        SCCNodes.push_back(i);
                    }
                    SCCNodes.push_back(nodeID);
                    SCCs[index] = SCCNodes;
                    index++;
                    }
            }
            return SCCs;
        }

        void interpret() {
            evalSchemes();
            evalFacts();
            cout << "Dependency Graph" << endl;
            cout << makeGraph(Datalog.getRules(), false).toString() << endl;
            cout << "Rule Evaluation" << endl;
            evalRules();
            cout << endl << "Query Evaluation" << endl;
            vector<Relation> evaluatedQueries = evalQueries();
            outputQueries(evaluatedQueries);
        }

        void evalRules() {
            Graph reverseGraph = makeGraph(Datalog.getRules(), true); // get reverse graph
            stack<int> postOrder = DFS_Forest(reverseGraph); // get post Order
            Graph dependencyGraph = makeGraph(Datalog.getRules(), false); // make dependency graph
            map<int, vector<int>> SCCs = findSCCs(postOrder, dependencyGraph); // find SCCs
            // Optional for viewing SCCs
            // for (const auto& pair : SCCs) {  // range-based for loop
            //     cout << "Index: " << pair.first << " -> ";
            //     for (int nodeID : pair.second) {
            //         cout << nodeID << " ";  // Print the values in the vector
            //     }
            //     cout << endl;
            // }

            for (auto& pair : SCCs) {
                // For printing purposes
                if (pair.second.size() > 0) {
                    cout << "SCC: ";
                    for (size_t i = 0; i < pair.second.size() - 1; i++) {
                        cout << "R" << pair.second[i];
                        if (i + 2 != pair.second.size()) {
                            cout << ",";
                        }
                    }
                    cout << endl;
                }

                int passes = 1;


                /////////// FIX THIS (Something might have to be done with the SCC Indexes or lists to show determine if something is related to itself/////////////

                bool selfDepends = false;
                for (int NodeID : dependencyGraph.getNodes()[pair.second.back()].getAdjacentNodeIDs()) {
                    if (pair.second.back() == NodeID) {
                        selfDepends = true;
                        break;
                    }
                }

                if ((pair.second.size() == 2) && (selfDepends == false)) {
                    unordered_set<string> usedTuples;
                    for (auto& relPair : db.getRelations()) {
                        for (const Tuple& tuple : relPair.second.getTuples()) {
                            string key = relPair.first + ": " + tuple.toString(relPair.second.getScheme());
                            usedTuples.insert(relPair.first + ": " + tuple.toString(relPair.second.getScheme()));
                        }
                    }
                    
                    Relation ruleEvaluation = evalRule(Datalog.getRules()[pair.second.back()]);
                    Rule rule = Datalog.getRules()[pair.second.back()]; // Get the corresponding rule
                    cout << rule.toString() << "." << endl;

                    for (const Tuple& tuple : ruleEvaluation.getTuples()) {
                        string targetRelationName = rule.getHeadPredicate().getName();
                        auto& relationsMap = db.getRelations();
                        Relation& targetRelation = relationsMap[targetRelationName];
                        string tupleKey = targetRelationName + ": " + tuple.toString(targetRelation.getScheme());
                        if (usedTuples.find(tupleKey) != usedTuples.end()) {
                            continue;
                        }
                        cout << "  " << tuple.toString(targetRelation.getScheme()) << endl;
                        usedTuples.insert(tupleKey);
                        targetRelation.addTuple(tuple);
                    }
                } else {
                    // implement fixed-point algorithm
                    stringstream out;
                    vector<Rule> SccRules;
                    for (size_t i = 0; i < pair.second.size() - 1; i++) {
                        SccRules.push_back(Datalog.getRules()[pair.second[i]]);
                    }

                    while (true) {

                        unordered_set<string> usedTuples;
                        for (auto& relPair : db.getRelations()) {
                            for (const Tuple& tuple : relPair.second.getTuples()) {
                                usedTuples.insert(relPair.first + ": " + tuple.toString(relPair.second.getScheme()));
                            }
                        }

                        int tuplesBefore = 0;
                        int tuplesAfter = 0;
                        // Count the number of tuples before evaluation
                        for (auto& relPair : db.getRelations()) {
                            tuplesBefore += relPair.second.getTuples().size();
                        }

                        // Evaluate all rules in a single iteration
                        vector<Relation> currentEvaluations;
                        for (const Rule& rule : SccRules) {
                            currentEvaluations.push_back(evalRule(rule));
                        }

                        // Count the number of tuples after evaluation
                        for (auto& relPair : db.getRelations()) {
                            tuplesAfter += relPair.second.getTuples().size();
                        }

                        // Print out the rule and corresponding new tuples created
                        for (size_t i = 0; i < currentEvaluations.size(); ++i) {
                            Rule rule = SccRules[i]; // Get the corresponding rule
                            out << rule.toString() << "." << endl;
                            
                            for (const Tuple& tuple : currentEvaluations[i].getTuples()) {

                                string targetRelationName = rule.getHeadPredicate().getName();
                                auto& relationsMap = db.getRelations();
                                Relation& targetRelation = relationsMap[targetRelationName];

                                string tupleKey = targetRelationName + ": " + tuple.toString(targetRelation.getScheme());
                                if (usedTuples.find(tupleKey) != usedTuples.end()) {
                                    continue;
                                }
                                out << "  " << tuple.toString(targetRelation.getScheme()) << endl;
                                usedTuples.insert(tupleKey);
                                targetRelation.addTuple(tuple);
                            }
                        }

                        // If no new tuples were added, break the loop
                        if (tuplesBefore == tuplesAfter) {
                            break;
                        }
                        passes++;
                    }
                    cout << out.str();
                }
                cout << passes << " passes: ";
                for (size_t i = 0; i < pair.second.size() - 1; i++) {
                    cout << "R" << pair.second[i];
                    if (i + 2 != pair.second.size()) {
                        cout << ",";
                    }
                }
                cout << endl;
            }
        }

        Relation evalRule(Rule rule) {
            // *** Evaluate the predicates on the right-hand side of the rule ***
            vector<Predicate> bodyPredicates = rule.getBodyPredicates();
            Predicate headPredicate = rule.getHeadPredicate();
            if (bodyPredicates.empty()) {
                return Relation();
            }
            
            Relation resultRelation = evalQuery(bodyPredicates[0]);
            for (size_t i = 1; i < bodyPredicates.size(); i++) {
                Relation intermediateResult = evalQuery(bodyPredicates[i]);
                // Join the relations that result
                resultRelation = resultRelation.join(intermediateResult);
            }

            // *** Project the columns that appear in the head predicate ***
            vector<int> variableIndices;
            unordered_map<string, int> variableToIndex;
            Scheme resultScheme = resultRelation.getScheme();

            // Map each variable from the head predicate to its correct index in joinedRelation
            for (Parameter param : rule.getHeadPredicate().getParameters()) {
                if (!param.isConstant()) {  // Only consider variables
                    string variable = param.toString();

                    // Find the correct index in joinedRelation's scheme
                    for (size_t j = 0; j < resultScheme.size(); j++) {
                        if (resultScheme[j] == variable && variableToIndex.find(variable) == variableToIndex.end()) {
                            variableIndices.push_back(j);
                            variableToIndex[variable] = j;
                            break;  // Stop searching once the first occurrence is found
                        }
                    }
                }
            }

            // Project using the correctly mapped indices
            resultRelation = resultRelation.project(variableIndices);

            // *** Rename the relation to make it union-compatible ***
            vector<string> newScheme;
            for (Parameter& param : headPredicate.getParameters()) {
                newScheme.push_back(param.toString());
            }
            vector<int> indicies;
            for (size_t i = 0; i < headPredicate.getParameters().size(); i++) {
                indicies.push_back(i);
            }

            resultRelation = resultRelation.rename(indicies, newScheme);
            // *** Union with the relation in the database ***

            string targetRelationName = headPredicate.getName();

            auto& relationsMap = db.getRelations();

            if (relationsMap.count(targetRelationName)) {
                Relation& targetRelation = relationsMap[targetRelationName];
                for (const Tuple& tuple : resultRelation.getTuples()) {
                    targetRelation.addTuple(tuple);
                }
            }
            return resultRelation;
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
            auto& relationsMap = db.getRelations();
            Relation queryRelation = relationsMap[query.getName()];

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
            vector<int> indicies;
            for (size_t i = 0; i < seenVariables.size(); i++) {
                indicies.push_back(i);
            }
            queryRelation = queryRelation.rename(indicies, seenVariables);

            return queryRelation;
        }

};