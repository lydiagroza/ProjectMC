
#include "WonderLoader.h"
#include "Wonder.h"
#include "Game.h"
#include "CardBase.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <memory>
#include <unordered_map>

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
            }
            else if (isdigit(c)) {
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
static std::vector<std::function<void(Player&, Player&)>> parseWonderEffects(const std::string& s)
{
    std::vector<std::function<void(Player&, Player&)>> effects;
    if (s.empty()) return effects;

    std::stringstream ss(s);
    std::string effectStr;

    static const std::unordered_map<std::string, std::function<void(Player&, Player&)>> effectMap = {
        {"add_coins3", [](Player& p, Player& o) { p.addCoins(3); }},
        {"decreaseCoins3", [](Player& p, Player& o) { o.decreaseCoins(3); }},
        {"replayTurn", [](Player& p, Player& o) { p.set_discountedResource(0); /* placeholder for replay turn logic */ }},
        {"add_VictoryPoint3", [](Player& p, Player& o) { p.add_Points(Points::Victory, 3); }},
        {"wood/stone/clay", [](Player& p, Player& o) { p.add_Resource(Resource::Wood, 1); p.add_Resource(Resource::Stone, 1); p.add_Resource(Resource::Clay, 1); }},
        {"add_VictoryPoints4", [](Player& p, Player& o) { p.add_Points(Points::Victory, 4); }},
        {"add_coins12", [](Player& p, Player& o) { p.addCoins(12); }},
        {"discardOpponentGrayCard", [](Player& p, Player& o) {
            Board& board = Game::currentGame->getBoard();
            const auto& opponentInventory = o.getInventory();
            std::vector<std::shared_ptr<CardBase>> grayCards;
            if (opponentInventory.count(Color::Gray)) {
                grayCards = opponentInventory.at(Color::Gray);
            }

            if (grayCards.empty()) {
                std::cout << "Opponent has no gray cards to discard." << std::endl;
                return;
            }

            std::cout << "Choose a gray card to discard from your opponent:" << std::endl;
            for (int i = 0; i < (int)grayCards.size(); ++i) {
                std::cout << i + 1 << ": " << *grayCards[i] << std::endl;
            }

            int choice = -1;
            // UI should provide choice; placeholder here
            if (choice > 0 && choice <= (int)grayCards.size()) {
                auto selectedCard = grayCards[choice - 1];
                o.removeCardFromInventory(selectedCard);
                board.addCardToDiscardPile(selectedCard);
                std::cout << "Discarded " << selectedCard->getName() << " from opponent." << std::endl;
            } else {
                std::cout << "No choice made (UI required) or invalid choice." << std::endl;
            }
        }},

        {"add_MilitaryPoint1", [](Player& p, Player& o) { p.add_Points(Points::Military, 1); }},
        {"buildDiscardedCard", [](Player& p, Player& o) {
            Board& board = Game::currentGame->getBoard();
            const auto& discardPile = board.getDiscardPile();

            if (discardPile.empty()) {
                std::cout << "Discard pile is empty." << std::endl;
                return;
            }

            std::cout << "Choose a card to build from the discard pile:" << std::endl;
            for (int i = 0; i < (int)discardPile.size(); ++i) {
                std::cout << i + 1 << ": " << *discardPile[i] << std::endl;
            }

            int choice = -1;
            // UI should provide choice; placeholder here

            if (choice > 0 && choice <= (int)discardPile.size()) {
                auto selectedCard = discardPile[choice - 1];
                p.addCardToInventory(selectedCard);
                board.removeCardFromDiscardPile(selectedCard);
                std::cout << "Built " << selectedCard->getName() << " from discard pile." << std::endl;
            } else {
                std::cout << "No choice made (UI required) or invalid choice." << std::endl;
            }
        }},

        {"add_VictoryPoint2", [](Player& p, Player& o) { p.add_Points(Points::Victory, 2); }},
        {"papyrus/glass", [](Player& p, Player& o) { p.add_Resource(Resource::Papyrus, 1); p.add_Resource(Resource::Glass, 1); }},
        {"add_VictoryPoints9", [](Player& p, Player& o) { p.add_Points(Points::Victory, 9); }},
        {"chooseProgressToken", [](Player& p, Player& o) {
            Board& board = Game::currentGame->getBoard();
            auto availableTokens = board.getAvailableProgressTokens();

            if (availableTokens.empty()) {
                std::cout << "No available progress tokens." << std::endl;
                return;
            }

            std::cout << "Choose a progress token:" << std::endl;
            for (int i = 0; i < (int)availableTokens.size(); ++i) {
                std::cout << i + 1 << ": " << *availableTokens[i] << std::endl;
            }

            int choice = -1;
            // UI should provide choice; placeholder here

            if (choice > 0 && choice <= (int)availableTokens.size()) {
                auto selectedToken = availableTokens[choice - 1];
                for (const auto& effect : selectedToken->getEffects()) {
                    effect(p, o);
                }
                board.removeAvailableProgressToken(selectedToken);
                std::cout << "You chose the " << selectedToken->getName() << " token." << std::endl;
            } else {
                std::cout << "No choice made (UI required) or invalid choice." << std::endl;
            }
        }},
        {"add_coins6", [](Player& p, Player& o) { p.addCoins(6); }},
        {"add_VictoryPoints", [](Player& p, Player& o) { p.add_Points(Points::Victory, 1); }},
        {"add_MilitaryPoints2", [](Player& p, Player& o) { p.add_Points(Points::Military, 2); }},
        {"discardOpponentBrownCard", [](Player& p, Player& o) {
            Board& board = Game::currentGame->getBoard();
            auto opponentInventory = o.getInventory();
            std::vector<std::shared_ptr<CardBase>> brownCards;
            if (opponentInventory.count(Color::Brown)) {
                brownCards = opponentInventory.at(Color::Brown);
            }

            if (brownCards.empty()) {
                std::cout << "Opponent has no brown cards to discard." << std::endl;
                return;
            }

            std::cout << "Choose a brown card to discard from your opponent:" << std::endl;
            for (int i = 0; i < (int)brownCards.size(); ++i) {
                std::cout << i + 1 << ": " << *brownCards[i] << std::endl;
            }

            int choice = -1;
            // UI should provide choice; placeholder here

            if (choice > 0 && choice <= (int)brownCards.size()) {
                auto selectedCard = brownCards[choice - 1];
                o.removeCardFromInventory(selectedCard);
                board.addCardToDiscardPile(selectedCard);
                std::cout << "Discarded " << selectedCard->getName() << " from opponent." << std::endl;
            } else {
                std::cout << "No choice made (UI required) or invalid choice." << std::endl;
            }
        }}
    };

    while (std::getline(ss, effectStr, ';')) {
        // Remove whitespace and quotes
        effectStr.erase(std::remove_if(effectStr.begin(), effectStr.end(), ::isspace), effectStr.end());
        effectStr.erase(std::remove(effectStr.begin(), effectStr.end(), '"'), effectStr.end());

        auto it = effectMap.find(effectStr);
        if (it != effectMap.end()) {
            effects.push_back(it->second);
        } else {
            std::cerr << "Unknown wonder effect: " << effectStr << std::endl;
        }
    }

    return effects;
}

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

        auto wonder = std::make_shared<Wonder>(name, id, cost);

        auto effects = parseWonderEffects(effectsStr);
        for (auto& ef : effects)
            wonder->addEffect(ef);
        wonders.push_back(wonder);
    }
    return wonders;
}