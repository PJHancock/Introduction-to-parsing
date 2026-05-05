#include "Token.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

using namespace std;

// Structure Classes
class Parameter {
    private:
        string name;
    public:
        // Constructor
        Parameter(string name) : name(name){};

        bool isConstant() {
            // check if it is string or ID
            if (isalpha(name[0])) { // ID
                return false;
            } else { // String
                return true;
            }
        }

        string getName() {
            return name;
        }

        string toString() {
            return name;
        }
};

class Predicate {
    private:
        string name;
        vector<Parameter> parameters;
    public:
        // Constructor
        Predicate(string name) : name(name){};

        void addparameter(Parameter parameter) {
            parameters.push_back(parameter);
        }

        vector<Parameter> getParameters() {
            return parameters;
        }

        string getName() {
            return name;
        }

        string toString() {
            stringstream out;
            out << name << "("; 
            for (size_t i = 0; i < parameters.size(); i++) {
                out << parameters[i].toString();
                if (i < parameters.size()-1) {
                    out << ',';
                }
            }
            out << ")";
            return out.str();
        }
};

class Rule {
    private:
        Predicate headPredicate;
        vector<Predicate> bodyPredicates;
    public:
        // Constructor
        Rule(Predicate headPredicate) : headPredicate(headPredicate){};

        void addPredicate(Predicate predicate) {
            bodyPredicates.push_back(predicate);
        }

        string toString() {
            stringstream out;
            out << headPredicate.toString() << " :- ";
            for (size_t i = 0; i < bodyPredicates.size(); i++) {
                out << bodyPredicates[i].toString();
                if (i < bodyPredicates.size()-1) {
                    out << ',';
                }
            }
            return out.str();
        }
};

class DatalogProgram {
    private:
        vector<Predicate> schemesList;
        vector<Predicate> factsList;
        vector<Rule> rulesList;
        vector<Predicate> queriesList;
        set<string> domainList;

    public:

        vector<Predicate> getSchemes() {
            return schemesList;
        }

        vector<Predicate> getFacts() {
            return factsList;
        }

        vector<Rule> getRules() {
            return rulesList;
        }

        vector<Predicate> getQueries() {
            return queriesList;
        }

        void addScheme(Predicate scheme) {
            schemesList.push_back(scheme);
        }

        void addFact(Predicate fact) {
            factsList.push_back(fact);
            for (Parameter& param : fact.getParameters()) {  // Use a range-based for loop
                if (param.toString().at(0) == '\'') {  // Assuming there is an isString() function
                    domainList.insert(param.toString());
                }
            }
        }

        void addRule(Rule rule) {
            rulesList.push_back(rule);
        }

        void addQuery(Predicate query) {
            queriesList.push_back(query);
        }

        string toString() {
            stringstream out;
            out << "Schemes(" << schemesList.size() << "):\n";
            for (size_t i = 0; i < schemesList.size(); i++) {
                out << "  " << schemesList[i].toString() << "\n";
            }
            out << "Facts(" << factsList.size() << "):\n";
            for (size_t i = 0; i < factsList.size(); i++) {
                out << "  " << factsList[i].toString() << ".\n";
            }
            out << "Rules(" << rulesList.size() << "):\n";
            for (size_t i = 0; i < rulesList.size(); i++) {
                out << "  " << rulesList[i].toString() << ".\n";
            }
            out << "Queries(" << queriesList.size() << "):\n";
            for (size_t i = 0; i < queriesList.size(); i++) {
                out << "  " << queriesList[i].toString() << "?\n";
            }
            out << "Domain(" << domainList.size() << "):\n";
            for (set<string>::iterator it = domainList.begin(); it != domainList.end(); it++) {
                out << "  " << *it << "\n";
            }
            return out.str();
        }
};

class Parser {
    private:
        vector<Token> tokens;
    public:
        // Constructor
        Parser(const vector<Token>& tokens) : tokens(tokens) { };
        
        // main function to parse program
        
        DatalogProgram parse() {
            try {
                DatalogProgram DP;
                datalogProgram(DP);
                return DP;
            } catch (Token error){
                cout << "Failure!\n  " << tokens.at(0).toString() << endl;
                throw error;
            }
        }

        // Helper Functions

        TokenType tokenType() {
            return tokens.at(0).getType();
        }

        void advanceToken() {
            tokens.erase(tokens.begin());

        }
        void throwError() {
            throw tokens.at(0);
        }

        void match(TokenType t) {
            // cout << "match: " << t << endl;
            while (tokens.at(0).getType() == COMMENT) {
                advanceToken();
            }
            if (tokens.at(0).getType() == t) {
                advanceToken();
            } else {
                throwError();
            }
        }

        // Grammar Rules

        void datalogProgram(DatalogProgram& DP){
            if (tokenType() == SCHEMES) {
                set<string> domainList;
                match(SCHEMES);
                match(COLON);
                Predicate firstScheme = scheme(DP);
                DP.addScheme(firstScheme);
                vector<Predicate> schemesList = schemeList(DP);
                for (const Predicate& scheme : schemesList) {
                    DP.addScheme(scheme);
                }
                match(FACTS);
                match(COLON);
                vector<Predicate> factsList = factList(DP);
                for (const Predicate& fact : factsList) {
                    DP.addFact(fact);
                }
                match(RULES);
                match(COLON);
                vector<Rule> rulesList = ruleList(DP);
                for (const Rule& rule : rulesList) {
                    DP.addRule(rule);
                }
                match(QUERIES);
                match(COLON);
                Predicate firstQuery = query(DP);
                DP.addQuery(firstQuery);
                vector<Predicate> queriesList = queryList(DP);
                for (const Predicate& query : queriesList) {
                    DP.addQuery(query);
                }
                match(END);
            } else {
                throwError();
            }
        }

        vector<Predicate> schemeList(DatalogProgram& DP) {
            vector<Predicate> schemes;
            if (tokenType() == ID) {
                Predicate newScheme = scheme(DP);
                schemes.push_back(newScheme);
                vector<Predicate> remainingSchemes = schemeList(DP);
                schemes.insert(schemes.end(), remainingSchemes.begin(), remainingSchemes.end());
            } else {
                // lambda
            }
            return schemes;
        }

        vector<Predicate> factList(DatalogProgram& DP) {
            vector<Predicate> facts;
            if (tokenType() == ID) {
                Predicate newFact = fact(DP);
                facts.push_back(newFact);
                vector<Predicate> remainingFacts = factList(DP);
                facts.insert(facts.end(), remainingFacts.begin(), remainingFacts.end());
            } else {
                // lambda
            }
            return facts;
        }

        vector<Rule> ruleList(DatalogProgram& DP) {
            vector<Rule> rules;
            if (tokenType() == ID) {
                Rule newRule = rule(DP);
                rules.push_back(newRule);
                vector<Rule> remainingRules = ruleList(DP);
                rules.insert(rules.end(), remainingRules.begin(), remainingRules.end());
            } else {
                // lambda
            }
            return rules;
        }

        vector<Predicate> queryList(DatalogProgram& DP) {
            vector<Predicate> queries;
            if (tokenType() == ID) {
                Predicate newQuery = query(DP);
                queries.push_back(newQuery);
                vector<Predicate> remainingQueries = queryList(DP);
                queries.insert(queries.end(), remainingQueries.begin(), remainingQueries.end());
            } else {
                // lambda
            }
            return queries;
        }

        Predicate scheme(DatalogProgram& DP) {
            if (tokenType() != ID) {
                throwError();
            }
            string name = tokens.at(0).getValue();
            match(ID);
            match(LEFT_PAREN);
            string firstID = tokens.at(0).getValue();
            match(ID);
            vector<string> idsList = idList(DP);
            match(RIGHT_PAREN);
            Predicate scheme(name);
            scheme.addparameter(firstID);
            for (const string& id : idsList) {
                scheme.addparameter(id);
            }
            return scheme;
        }

        Predicate fact(DatalogProgram& DP) {
            if (tokenType() != ID) {
                throwError();
            }
            string name = tokens.at(0).getValue();
            match(ID);
            match(LEFT_PAREN);
            Parameter firstParameter = Parameter(tokens.at(0).getValue());
            match(STRING);
            vector<string> stringsList = stringList(DP);
            Predicate fact(name);
            fact.addparameter(firstParameter);
            for (const Parameter& strings : stringsList) {
                fact.addparameter(Parameter(strings));
            }
            match(RIGHT_PAREN);
            match(PERIOD);
            return fact;
        }

        Rule rule(DatalogProgram& DP) {
            if (tokenType() != ID) {
                throwError();
            }
            Predicate head = headPredicate(DP);
            match(COLON_DASH);
            Predicate firstPredicate = predicate(DP);
            Rule newRule(head);
            newRule.addPredicate(firstPredicate);
            vector<Predicate> bodyPredicates = predicateList(DP);
            for (const Predicate& bodyPredicate : bodyPredicates) {
                newRule.addPredicate(bodyPredicate);
            }
            match(PERIOD);
            return newRule;
        }

        Predicate query(DatalogProgram& DP) {
            if (tokenType() != ID) {
                throwError();
            }
            Predicate query = predicate(DP);
            match(Q_MARK);
            return query;
        }

        Predicate headPredicate(DatalogProgram& DP) {
            if (tokenType() != ID) {
                throwError();
            }
            string name = tokens.at(0).getValue();
            match(ID);
            match(LEFT_PAREN);
            vector<Parameter> params;
            params.push_back(Parameter(tokens.at(0).getValue()));
            match(ID);
            vector<string> idsList = idList(DP);
            for (const Parameter& id : idsList) {
                params.push_back(Parameter(id));
            }
            match(RIGHT_PAREN);
            Predicate headPredicate(name);
            for (const Parameter& param : params) {
                headPredicate.addparameter(param);
            }
            return headPredicate;
        }

        Predicate predicate(DatalogProgram& DP) {
            if (tokenType() != ID) {
                throwError();
            }
            string name = tokens.at(0).getValue();
            match(ID);
            match(LEFT_PAREN);
            vector<Parameter> params;
            Parameter firstParam = parameter(DP);
            params.push_back(firstParam);
            vector<Parameter> remainingParams = parameterList(DP);
            for (const Parameter& param : remainingParams) {
                params.push_back(param);
            }
            match(RIGHT_PAREN);
            Predicate headPredicate(name);
            for (const Parameter& param : params) {
                headPredicate.addparameter(param);
            }
            return headPredicate;
        }

        vector<Predicate> predicateList(DatalogProgram& DP) {
            vector<Predicate> predicates;
            if (tokenType() == COMMA) {
                match(COMMA);
                Predicate newPredicate = predicate(DP);
                predicates.push_back(newPredicate);
                vector<Predicate> remainingPredicates = predicateList(DP);
                predicates.insert(predicates.end(), remainingPredicates.begin(), remainingPredicates.end());
            } else {
                // lambda
            }
            return predicates;
        }

        vector<Parameter> parameterList(DatalogProgram& DP) {
            vector<Parameter> parameters;
            if (tokenType() == COMMA) {
                match(COMMA);
                Parameter newParameter = parameter(DP);
                parameters.push_back(newParameter);
                vector<Parameter> remainingParameters = parameterList(DP);
                parameters.insert(parameters.end(), remainingParameters.begin(), remainingParameters.end());
            } else {
                // lambda
            }
            return parameters;
        }

        vector<string> stringList(DatalogProgram& DP) {
            vector<string> strings;
            if (tokenType() == COMMA) {
                match(COMMA);
                string newString = tokens.at(0).getValue();
                match(STRING);
                strings.push_back(newString);
                vector<string> remainingStrings = stringList(DP);
                strings.insert(strings.end(), remainingStrings.begin(), remainingStrings.end());
            } else {
                // lambda
            }
            return strings;
        }

        vector<string> idList(DatalogProgram& DP) {
            vector<string> IDs;
            if (tokenType() == COMMA) {
                match(COMMA);
                string newID = tokens.at(0).getValue();
                match(ID);
                IDs.push_back(newID);
                vector<string> remainingIDs = idList(DP);
                IDs.insert(IDs.end(), remainingIDs.begin(), remainingIDs.end());
            } else {
                // lambda
            }
            return IDs;
        }

        Parameter parameter(DatalogProgram& DP) {
            if (!(tokenType() == STRING || tokenType() == ID)) {
                throwError();
            }
            if (tokenType() == STRING) {
                string value = tokens.at(0).getValue();
                match(STRING);
                return Parameter(value);
            } else { // (tokenType() == ID)
                string value = tokens.at(0).getValue();
                match(ID);
                return Parameter(value);
            }
        }
};