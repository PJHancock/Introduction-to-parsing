#include <iostream>
#include "Token.h"
#include "Scanner.h"
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
    vector<string> token_list = s.scanTokens();
    inputFile.close();

    for (int i = 0; i < token_list.size(); i++) {
        cout <<  token_list[i] << endl;
    }
    cout << "Total Tokens = " << token_list.size() << endl;
    return 0;
}

// test change