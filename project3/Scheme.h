#pragma once
#include <vector>

using namespace std;

class Scheme : public vector<string> {
    public:
        Scheme() = default;

        Scheme(vector<string> names) : vector<string>(names) { };

};