#include "Wonder.h"
#include "Player.h"
#include <sstream>
#include <unordered_map>
#include <iostream>
#include "CardLoader.h"
#include "Game.h"

class Player;

std::map<Resource, uint8_t> Wonder::parseCost(const std::string& s) {
    std::map<Resource, uint8_t> cost;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ';')) {
        std::string resourceName;
        int amount = 0;
        for (char c : item) {
            if (isalpha(c)) {
                resourceName += c;
            } else if (isdigit(c)) {
                amount = amount * 10 + (c - '0');
            }
        }

        if (amount == 0) amount = 1; // Default la 1 daca nu este specificat un amount

        if (resourceName == "wood") cost[Resource::Wood] = amount;
        else if (resourceName == "clay") cost[Resource::Clay] = amount;
        else if (resourceName == "stone") cost[Resource::Stone] = amount;
        else if (resourceName == "glass") cost[Resource::Glass] = amount;
        else if (resourceName == "papyrus") cost[Resource::Papyrus] = amount;
        else if (resourceName == "coin") cost[Resource::Coin] = amount;
    }
    return cost;
}

std::vector<std::function<void(Player&, Player&)>> Wonder::parseEffects(const std::string& s) {
    std::vector<std::function<void(Player&, Player&)>> effects;
    std::stringstream ss(s);
    std::string effectStr;

    static const std::unordered_map<std::string, std::function<void(Player&, Player&)>> effectMap = {
        {"add_coins3", [](Player& p, Player& o) { p.addCoins(3); }},
        {"decreaseCoins3", [](Player& p, Player& o) { o.decreaseCoins(3); }},
        {"replayTurn", [](Player& p, Player& o) { Game::switchTurn(); }},
        {"add_VictoryPoint3", [](Player& p, Player& o) { p.add_Points(Points::Victory, 3); }},
        {"wood/stone/clay", [](Player& p, Player& o) { p.add_Resource(Resource::Wood, 1); p.add_Resource(Resource::Stone, 1); p.add_Resource(Resource::Clay, 1); }},
        {"add_VictoryPoints4", [](Player& p, Player& o) { p.add_Points(Points::Victory, 4); }},
        {"add_coins12", [](Player& p, Player& o) { p.addCoins(12); }},
        {"discardOpponentGrayCard", [](Player& p, Player& o) { /* discardOpponentGrayCard*/ std::cout << "Discard opponent gray card not implemented." << std::endl; }},
        {"add_MilitaryPoint1", [](Player& p, Player& o) { p.add_Points(Points::Military, 1); }},
        {"buildDiscardedCard", [](Player& p, Player& o) { /* builDiscardedCard */ std::cout << "Build discarded card not implemented." << std::endl; }},
        {"add_VictoryPoint2", [](Player& p, Player& o) { p.add_Points(Points::Victory, 2); }},
        {"papyrus/glass", [](Player& p, Player& o) { p.add_Resource(Resource::Papyrus, 1); p.add_Resource(Resource::Glass, 1); }},
        {"add_VictoryPoints9", [](Player& p, Player& o) { p.add_Points(Points::Victory, 9); }},
        {"chooseProgressToken", [](Player& p, Player& o) {/*chooseProgressToken */ std::cout << "Choose progress token not implemented." << std::endl; }},
        {"add_coins6", [](Player& p, Player& o) { p.addCoins(6); }},
        {"add_VictoryPoints", [](Player& p, Player& o) { p.add_Points(Points::Victory, 1); }},
        {"add_MilitaryPoints2", [](Player& p, Player& o) { p.add_Points(Points::Military, 2); }},
        {"discardOpponentBrownCard", [](Player& p, Player& o) { /* discardOpponentBrownCard */ std::cout << "Discard opponent brown card not implemented." << std::endl; }}
    };

    while (std::getline(ss, effectStr, ';')) {
        if (effectMap.count(effectStr)) {
            effects.push_back(effectMap.at(effectStr));
        } else {
            std::cerr << "Unknown effect: " << effectStr << std::endl;
        }
    }
    return effects;
}