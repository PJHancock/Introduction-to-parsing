#include "Token.h"
#include "Parser.h"
#include "Scanner.h"
#include <vector>
#include <iostream>
#include <fstream>

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
    p.parse();
}