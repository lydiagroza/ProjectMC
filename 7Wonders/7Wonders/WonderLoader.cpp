#include "WonderLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

std::vector<std::shared_ptr<Wonder>> WonderLoader::loadWonders(const std::string& filename) {
    std::vector<std::shared_ptr<Wonder>> wonders;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Eroare, nu s-a putut deschide fisierul " << filename << std::endl;
        return wonders;
    }

    std::string line;
    getline(file, line); // Skip header

    auto trim = [](std::string s) {
        s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
        s.erase(remove(s.begin(), s.end(), '"'), s.end());
        return s;
    };

    while (getline(file, line)) {
        if (line.empty() || line.find_first_not_of(" \t\n\v\f\r,") == std::string::npos) {
            continue;
        }
        std::stringstream ss(line);
        std::string name, idStr, costStr, effectsStr;

        std::getline(ss, name, ',');
        std::getline(ss, idStr, ',');
        std::getline(ss, costStr, ',');
        std::getline(ss, effectsStr, '\n');

        name = trim(name);
        idStr = trim(idStr);
        costStr = trim(costStr);
        effectsStr = trim(effectsStr);

        uint16_t id = static_cast<uint16_t>(stoi(idStr));
        std::map<Resource, uint8_t> cost = Wonder::parseCost(costStr);

        auto wonder = make_shared<Wonder>(name, id, cost);

        auto effects = Wonder::parseEffects(effectsStr);
        for (auto& ef : effects)
            wonder->addEffect(ef);
        wonders.push_back(wonder);
    }
    return wonders;
}