#pragma once
#include <string>
#include <sstream>
#include <cctype>
using namespace std;

enum TokenType {
    COMMA, PERIOD, Q_MARK, LEFT_PAREN, RIGHT_PAREN,
    COLON, COLON_DASH, MULTIPLY, ADD, SCHEMES, FACTS,
    RULES, QUERIES, ID, STRING, COMMENT, UNDEFINED, END
};

class Token {
    private:
        TokenType type;
        string value;
        int line;
    public:
        Token(TokenType t, string v, int l) : type(t), value(v), line(l) {}
    
        string toString() const {
            stringstream out;
            out << "(" << typeName(type) << "," << "\"" << value << "\"" << "," << line << ")";
            return out.str();
        }

        TokenType getType() {
            return type;
        }

        string getValue() {
            return value;
        }

        string typeName(TokenType type) const {
            // return the correct string for each TokenType value
            if (type == 0) return "COMMA";
            if (type == PERIOD) return "PERIOD";
            if (type == Q_MARK) return "Q_MARK";
            if (type == LEFT_PAREN) return "LEFT_PAREN";
            if (type == RIGHT_PAREN) return "RIGHT_PAREN";
            if (type == COLON) return "COLON";
            if (type == COLON_DASH) return "COLON_DASH";
            if (type == MULTIPLY) return "MULTIPLY";
            if (type == ADD) return "ADD";
            if (type == SCHEMES) return "SCHEMES";
            if (type == FACTS) return "FACTS";
            if (type == RULES) return "RULES";
            if (type == QUERIES) return "QUERIES";
            if (type == ID) return "ID";
            if (type == STRING) return "STRING";
            if (type == COMMENT) return "COMMENT";
            if (type == UNDEFINED) return "UNDEFINED";
            if (type == END) return "END";
            else return "UNDEFINED";
        }
};