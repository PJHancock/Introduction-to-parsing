
#include <iostream>
#include "Scanner.h"
#include "Node.h"
#include "Graph.h"
#include "Parser.h"
#include "Interpreter.h"

using namespace std;

int main(int argc, char* argv[]) {
  
  string filename = argv[1];
  ifstream inputFile(filename);
  if (!inputFile) {
      cerr << "Error: Unable to open file!" << endl;
      return 1;
  }
  string file_content((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
  Scanner s = Scanner(file_content);
  vector<Token> token_list = s.scanTokens();
  inputFile.close();
  Parser p = Parser(token_list);
  DatalogProgram DP = p.parse();
  Interpreter I = Interpreter(DP);
  I.interpret();
}

// int main() {

//   // predicate names for fake rules
//   // first is name for head predicate
//   // second is names for body predicates
//   pair<string,vector<string>> ruleNames[] = {
//     { "Sibling", { "Sibling" } },
//     { "Ancestor", { "Ancestor", "Parent" } },
//     { "Ancestor", { "Parent" } },
//   };

//   vector<Rule> rules;

//   for (auto& rulePair : ruleNames) {
//     string headName = rulePair.first;
//     Rule rule = Rule(Predicate(headName));
//     vector<string> bodyNames = rulePair.second;
//     for (auto& bodyName : bodyNames)
//       rule.addPredicate(Predicate(bodyName));
//     rules.push_back(rule);
//   }

//   Graph reverseGraph = Interpreter::makeGraph(rules, true); // get reverse graph
//   cout << reverseGraph.toString() << endl;
//   stack<int> postOrder = Interpreter::DFS_Forest(reverseGraph);
//   Graph dependencyGraph = Interpreter::makeGraph(rules, false);
//   map<int, vector<int>> SCCs = Interpreter::findSCCs(postOrder, dependencyGraph);
//   for (const auto& pair : SCCs) {  // range-based for loop
//       cout << "Index: " << pair.first << " -> ";
//       for (int nodeID : pair.second) {
//           cout << nodeID << " ";  // Print the values in the vector
//       }
//       cout << endl;
//   }
// }