#include "WonderLoader.h"
#include "Game.h"
#include "CardBase.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <unordered_map>

std::map<Resource, uint8_t> WonderLoader::parseCost(const std::string& s) {
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
    std::string token;

    static const std::unordered_map<std::string, std::function<void(Player&, Player&)>> effectMap = {
        {"add_resource_coin3", [](Player& p, Player& o) { p.addResource(Coin,3); }},
        {"add_resource_coin6", [](Player& p, Player& o) { p.addResource(Coin,6); }},
        {"add_resource_coin12", [](Player& p, Player& o) { p.addResource(Coin, 12); }},
        {"decreaseCoin3", [](Player& p, Player& o) { o.decreaseCoins(3); }},

        {"replayTurn", [](Player& p, Player& o) { p.setHasExtraTurn(true);}},
        {"wood/stone/clay", [](Player& p, Player& o) { p.addResource(Resource::Wood, 1); p.addResource(Resource::Stone, 1); p.addResource(Resource::Clay, 1); }},
        {"papyrus/glass", [](Player& p, Player& o) { p.addResource(Resource::Papyrus, 1); p.addResource(Resource::Glass, 1); }},
        
        
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
            }
        else {
         std::cout << "No choice made (UI required) or invalid choice." << std::endl;
        }
        }},


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
            }
        else {
         std::cout << "No choice made (UI required) or invalid choice." << std::endl;
        }
        }},

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
            }
        else {
         std::cout << "No choice made (UI required) or invalid choice." << std::endl;
        }
        }},

    };

    const std::string vpPrefix = "add_VictoryPoint";
    const std::string mpPrefix = "add_MilitaryPoint";

    while (getline(ss, token, ';')) {
        int ok = 0;
        token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());

        if (token.rfind(vpPrefix, 0) == 0) {
            int amount = stoi(token.substr(vpPrefix.size()));
            effects.push_back([amount](Player& p, Player& o) { p.add_Points(Points::Victory, static_cast<std::uint8_t>(amount)); });
            ok = 1;
            continue;
        }
        else if (token.rfind(mpPrefix, 0) == 0) {
            int amount = stoi(token.substr(mpPrefix.size()));
            effects.push_back([amount](Player& p, Player& o) { p.add_Points(Points::Military, static_cast<std::uint8_t>(amount)); });
            ok = 1;
            continue;
        }

        auto it = effectMap.find(token);
        if (it != effectMap.end()) {
            effects.push_back(it->second);
            ok = 1;
            continue;
        }
        if (ok == 0)
            std::cout << "Effect " << token << " unknown" << std::endl;
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
        s.erase(remove_if(s.begin(), s.end(), [](unsigned char c) { return std::isspace(c); }), s.end());
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
        std::map<Resource, uint8_t> cost = parseCost(costStr);

        auto wonder = std::make_shared<Wonder>(name, id, cost);

        auto effects = parseWonderEffects(effectsStr);
        for (auto& ef : effects)
            wonder->addEffect(ef);
        wonders.push_back(wonder);
    }
    return wonders;
}