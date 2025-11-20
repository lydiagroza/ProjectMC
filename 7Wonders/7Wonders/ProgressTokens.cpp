#include "ProgressTokens.h"
#include <sstream>
#include <unordered_map>
#include <iostream>
#include "CardBase.h"

// Forward declaration to avoid circular dependency
class Player;

std::vector<std::function<void(Player&, Player&)>> ProgressToken::parseEffects(const std::string& s) {
    std::vector<std::function<void(Player&, Player&)>> effects;
    std::stringstream ss(s);
    std::string effectStr;

    static const std::unordered_map<std::string, std::function<void(Player&, Player&)>> effectMap = {
        {"add_coins6", [](Player& p, Player& o) { p.addCoins(6); }},
        {"add_VictoryPoint4", [](Player& p, Player& o) { p.add_Points(Points::Victory, 4); }},
        {"WondersDiscount", [](Player& p, Player& o) { /* Placeholder */ std::cout << "WondersDiscount not implemented." << std::endl; }},
        {"gainOpponentTradeCost", [](Player& p, Player& o) { /* Placeholder */ std::cout << "gainOpponentTradeCost not implemented." << std::endl; }},
        {"add_scientific_symbol_law", [](Player& p, Player& o) { p.add_ScientificSymbol(Scientific_Symbol::Scales); }},
        {"victoryPointsPerProgress", [](Player& p, Player& o) { /* Placeholder */ std::cout << "victoryPointsPerProgress not implemented." << std::endl; }},
        {"BlueCardDiscount", [](Player& p, Player& o) { /* Placeholder */ std::cout << "BlueCardDiscount not implemented." << std::endl; }},
        {"add_VictoryPoint7", [](Player& p, Player& o) { p.add_Points(Points::Victory, 7); }},
        {"add_MilitaryPoint1PerRedCard", [](Player& p, Player& o) { /* Placeholder */ std::cout << "add_MilitaryPoint1PerRedCard not implemented." << std::endl; }},
        {"WondersGetReplayTurn", [](Player& p, Player& o) { /* Placeholder */ std::cout << "WondersGetReplayTurn not implemented." << std::endl; }},
        {"add_coins4PerFreeCard", [](Player& p, Player& o) { /* Placeholder */ std::cout << "add_coins4PerFreeCard not implemented." << std::endl; }}
    };

    while (std::getline(ss, effectStr, ';')) {
        // Trim whitespace and quotes
        effectStr.erase(remove_if(effectStr.begin(), effectStr.end(), ::isspace), effectStr.end());
        effectStr.erase(remove(effectStr.begin(), effectStr.end(), '"'), effectStr.end());

        if (effectMap.count(effectStr)) {
            effects.push_back(effectMap.at(effectStr));
        } else {
            std::cerr << "Unknown progress token effect: " << effectStr << std::endl;
        }
    }
    return effects;
}
