#include <vector>
#include <iostream>
#include "Tuple.h"
#include "Scheme.h"
#include "Relation.h"
#include "Interpreter.h"
#include "Scanner.h"

using namespace std;

int main(int argc, char* argv[]) {

  // vector<string> names = { "ID", "Name", "Major" };

  // Scheme scheme(names);

  // Relation relation("student", scheme);

  // vector<string> values[] = {
  //   {"'42'", "'Ann'", "'CS'"},
  //   {"'32'", "'Bob'", "'CS'"},
  //   {"'64'", "'Ned'", "'EE'"},
  //   {"'16'", "'Jim'", "'EE'"},
  // };

  // for (auto& value : values) {
  //   Tuple tuple(value);
  //   cout << tuple.toString(scheme) << endl;
  //   relation.addTuple(tuple);
  // }

  // cout << "relation:" << endl;
  // cout << relation.toString();

  // Relation result = relation.select(2, "'CS'");

  // cout << "select Major='CS' result:" << endl;
  // cout << result.toString();

  // Relation result2 = result.project(1);

  // cout << "project Name:" << endl;
  // cout << result2.toString();

  // Relation result3 = result2.rename(0, "newName");

  // cout << "rename Name to newName:" << endl;
  // cout << result3.toString();


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
