#include "ProgressTokenLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include"Player.h"
#include "Game.h"
#include <algorithm>
#include <cctype>
std::vector<std::function<void(Player&, Player&)>> ProgressTokenLoader::parseEffects(const std::string& s) {

    std::vector<std::function<void(Player&, Player&)>> effects;
    std::stringstream ss(s);
    std::string effectStr;


    static const std::unordered_map<std::string, std::function<void(Player&, Player&)>> effectMap = {
        {"add_coins6", [](Player& p, Player&) { p.addResource(Coin, 6); }},
        {"add_VictoryPoint4", [](Player& p, Player&) { p.add_Points(Points::Victory, 4); }},
        {"WondersDiscount", [](Player& p, Player&) { p.setWonderDiscount(true); }},
        {"gainOpponentTradeCost", [](Player& p, Player&) {p.setGainsOpponentTradeCost(true); }},
        {"add_scientific_symbol_scales", [](Player& p, Player&) { 
            if (p.add_ScientificSymbol(Scientific_Symbol::Scales)) {
                if (Game::currentGame) Game::currentGame->handleProgressTokenChoice();
            }
        }},
        //aici trebuie cumva apelat la sfarsitul jocului nu in timpul jocului 
        {"victoryPointsPerProgress", [](Player& p, Player&) {p.setMathBonus(true); }},
        {"BlueCardDiscount", [](Player& p, Player&) { p.setBlueCardDiscount(true); }},
        {"add_VictoryPoint7", [](Player& p, Player&) { p.add_Points(Points::Victory, 7); }},
        /* aici la MilitaryPoints trebuie sa vedem cum facem, daca returnam cumva nr de carti spre board sau le adaugam undeva*/
        {"add_MilitaryPoint1PerRedCard", [](Player& p, Player&) { p.setExtraMilitary(true); }},
        {"WondersGetReplayTurn", [](Player& p, Player&) { p.setTheologyBonus(true); }},
        {"add_coins4PerFreeCard", [](Player& p, Player&) {

            p.setGetsCoinsForFreeCards(true); }}
    };

    while (std::getline(ss, effectStr, ';')) {

        effectStr.erase(remove_if(effectStr.begin(), effectStr.end(), ::isspace), effectStr.end());
        effectStr.erase(remove(effectStr.begin(), effectStr.end(), '"'), effectStr.end());

        if (effectMap.count(effectStr)) {
            effects.push_back(effectMap.at(effectStr));
        }
        else {
            std::cerr << "Unknown progress token effect: " << effectStr << std::endl;
        }
    }

    return effects;
}
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

        auto effects = parseEffects(effectsStr);
        for (auto& ef : effects)
            token->addEffect(ef);
        tokens.push_back(token);
    }
    return tokens;
}