#pragma once
#include <string>
#include "Token.h"
#include <cctype>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

class Scanner {
    private:
        string file_content;
        int line_num = 1;
    public:
        // Scanner constructor
        Scanner(string file_content) : file_content(file_content) {}

        // Scan all tokens from a line
        vector<string> scanTokens() {
            // TODO: Find ways to get around the non-permanent variables
            vector<string> token_list = {};
            int i = 0;
            while (i < file_content.size()) {
                // skip whitespace
                char curr_char = file_content[i];
                TokenType type = UNDEFINED;
                string value;
                while (i < file_content.size() && isspace(curr_char)) {
                    if (file_content[i] == '\n') ++line_num;
                    ++i;
                    curr_char = file_content[i];
                }
                int line = line_num;
                if (i >= file_content.size()) break;

                // handle single char tokens
                if (curr_char == ',') {
                    type = COMMA;
                    value = ",";
                    ++i;
                } else if (curr_char == '.') {
                    type = PERIOD;
                    value = ".";
                    ++i;
                } else if (curr_char == '?') {
                    type = Q_MARK;
                    value = "?";
                    ++i;      
                } else if (curr_char == '(') {
                    type = LEFT_PAREN;
                    value = "(";
                    ++i;      
                } else if (curr_char == ')') {
                    type = RIGHT_PAREN;
                    value = ")";
                    ++i;  
                } else if (curr_char == ':') {
                    if (file_content.size() > 1 && file_content[i+1] == '-') {
                        type = COLON_DASH;
                        value = ":-";
                        i += 2;      
                    } else {
                        type = COLON;
                        value = ":";
                        ++i;;      
                    }
                } else if (curr_char == '*') {
                    type = MULTIPLY;
                    value = "*";
                    ++i;      
                } else if (isalpha(curr_char)) {
                    type = ID;
                    int start = i;
                    while (i < file_content.size() && (isalnum(file_content[i]) || isdigit(file_content[i]) || file_content[i] == '_')) {
                        ++i;
                    }
                    value = file_content.substr(start, i - start);
                    if (value == "Schemes") type = SCHEMES;
                    else if (value == "Facts") type = FACTS;
                    else if (value == "Rules") type = RULES;
                    else if (value == "Queries") type = QUERIES;
                    else type = ID;
                } else if (curr_char == '#'){
                    type = UNDEFINED;
                    int start = i;
                    ++i;
                    if (file_content[i] == '|') {
                        while (i < file_content.size() && !(file_content[i] == '|' && file_content[i+1] == '#')) { 
                            if (file_content[i] == '\n') {
                                ++line_num;
                            }
                            cout << file_content[i] << endl;
                            ++i;
                        }
                        i += 2;
                        if (file_content[i-1] == '#' && file_content[i-2] == '|') {
                            type = COMMENT;
                        }
                    } else {
                        while (i < file_content.size() && (file_content[i] != '\n')) {
                            ++i;
                        }
                        type = COMMENT;
                    }
                    value = file_content.substr(start, i - start);
                    // Stop outputting comments
                    continue;
                } else if (curr_char == '\''){
                    int start = i;
                    ++i;
                    while (i < file_content.size() && ((file_content[i] != '\'') || (file_content[i] == '\'' && file_content[i+1] == '\''))) {
                        if (file_content[i] == '\n') {
                                ++line_num;
                            }
                        if (file_content[i] == '\'' && file_content[i+1] == '\'') {
                            ++i;
                        }
                        ++i;
                    }
                    ++i;
                    value = file_content.substr(start, i - start);
                    if (file_content[i-1] != '\'') {
                        type = UNDEFINED;
                    } else {
                        type = STRING;
                    }
                } else {
                    value = curr_char;
                    ++i;
                }
                token_list.push_back(Token(type, value, line).toString());
            }
            token_list.push_back(Token(END, "", line_num).toString());
            return token_list;
        }
};