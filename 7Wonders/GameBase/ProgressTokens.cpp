#include "ProgressTokens.h"
#include <sstream>
#include <unordered_map>
#include <iostream>
#include "CardBase.h"
#include "Player.h"
#include <algorithm>
#include <cctype>


class Player;

std::vector<std::function<void(Player&, Player&)>> ProgressToken::parseEffects(const std::string& s) {
    std::vector<std::function<void(Player&, Player&)>> effects;
    std::stringstream ss(s);
    std::string effectStr;
 

    static const std::unordered_map<std::string, std::function<void(Player&, Player&)>> effectMap = {
        {"add_coins6", [](Player& p, Player& o) { p.addResource(Coin,6); }},
        {"add_VictoryPoint4", [](Player& p, Player& o) { p.add_Points(Points::Victory, 4); }},
        {"WondersDiscount", [](Player& p, Player& o) { p.setWonderDiscount(true); }},
        {"gainOpponentTradeCost", [](Player& p, Player& o) { p.setGainsOpponentTradeCost(true); }},
        {"add_scientific_symbol_law", [](Player& p, Player& o) { p.add_ScientificSymbol(Scientific_Symbol::Law); }},
        {"victoryPointsPerProgress", [](Player& p, Player& o) {
            // Assumed 2 VP per progress token, as specified by the task.
            p.add_Points(Points::Victory, p.getProgressTokens().size() * 2);
        }},
        {"BlueCardDiscount", [](Player& p, Player& o) { p.setBlueCardDiscount(true); }},
        {"add_VictoryPoint7", [](Player& p, Player& o) { p.add_Points(Points::Victory, 7); }},
        {"add_MilitaryPoint1PerRedCard", [](Player& p, Player& o) { 
			if (p.getInventory().count(Color::Red)) {
				p.add_Points(Points::Military, p.getInventory().at(Color::Red).size());
			}
		}},
        {"WondersGetReplayTurn", [](Player& p, Player& o) { p.setHasExtraTurn(true); }},
        {"add_coins4PerFreeCard", [](Player& p, Player& o) { p.setGetsCoinsForFreeCards(true); }}
    };

    while (std::getline(ss, effectStr, ';')) {
       
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

std::ostream& operator<<(std::ostream& os, const ProgressToken& token)
{
    os << "Progress Token ID: " << token.getId() << ", Name: " << token.getName();
	return os;
}