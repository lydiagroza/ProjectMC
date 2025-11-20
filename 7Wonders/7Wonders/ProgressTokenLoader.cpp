#include "ProgressTokenLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

std::vector<std::shared_ptr<ProgressToken>> ProgressTokenLoader::loadProgressTokens(const std::string& filename) {
    using namespace std;
    vector<shared_ptr<ProgressToken>> tokens;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Eroare, nu s-a putut deschide fisierul " << filename << endl;
        return tokens;
    }

    string line;
    getline(file, line); // Skip header

    auto trim = [](string s) {
        s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
        s.erase(remove(s.begin(), s.end(), '"'), s.end());
        return s;
    };

    while (getline(file, line)) {
        stringstream ss(line);
        string name, idStr, effectsStr;

        getline(ss, name, ',');
        getline(ss, idStr, ',');
        getline(ss, effectsStr, '\n');

        name = trim(name);
        idStr = trim(idStr);
        effectsStr = trim(effectsStr);

        uint16_t id = static_cast<uint16_t>(stoi(idStr));

        auto token = make_shared<ProgressToken>(name, id);

        auto effects = ProgressToken::parseEffects(effectsStr);
        for (auto& ef : effects)
            token->addEffect(ef);
        tokens.push_back(token);
    }
    return tokens;
}
