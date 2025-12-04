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
        {"replayTurn", [](Player& p, Player& o) { /*???*/ }},
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
            for (int i = 0; i < grayCards.size(); ++i) {
                std::cout << i + 1 << ": " << *grayCards[i] << std::endl;
            }

            int choice = -1;
            //std::cin >> choice; // alege player ce carte sa dea discard, facem cumva in UI

            if (choice > 0 && choice <= grayCards.size()) {
                auto selectedCard = grayCards[choice - 1];
                o.removeCardFromInventory(selectedCard);
                board.addCardToDiscardPile(selectedCard);
                std::cout << "Discarded " << selectedCard->getName() << " from opponent." << std::endl;
            } else {
                std::cout << "Invalid choice." << std::endl;
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
            for (int i = 0; i < discardPile.size(); ++i) {
                std::cout << i + 1 << ": " << *discardPile[i] << std::endl;
            }

            int choice = -1;
            //std::cin >> choice;

            if (choice > 0 && choice <= discardPile.size()) {
                auto selectedCard = discardPile[choice - 1];
                p.addCardToInventory(selectedCard);
                board.removeCardFromDiscardPile(selectedCard);
                std::cout << "Built " << selectedCard->getName() << " from discard pile." << std::endl;
            } else {
                std::cout << "Invalid choice." << std::endl;
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
            for (int i = 0; i < availableTokens.size(); ++i) {
                std::cout << i + 1 << ": " << *availableTokens[i] << std::endl;
            }

            int choice = -1;
            //std::cin >> choice;

            if (choice > 0 && choice <= availableTokens.size()) {
                auto selectedToken = availableTokens[choice - 1];
                for (const auto& effect : selectedToken->getEffects()) {
                    effect(p, o);
                }
                board.removeAvailableProgressToken(selectedToken);
                std::cout << "You chose the " << selectedToken->getName() << " token." << std::endl;
            } else {
                std::cout << "Invalid choice." << std::endl;
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
            for (int i = 0; i < brownCards.size(); ++i) {
                std::cout << i + 1 << ": " << *brownCards[i] << std::endl;
            }

            int choice = -1;
            //std::cin >> choice;

            if (choice > 0 && choice <= brownCards.size()) {
                auto selectedCard = brownCards[choice - 1];
                o.removeCardFromInventory(selectedCard);
                board.addCardToDiscardPile(selectedCard);
                std::cout << "Discarded " << selectedCard->getName() << " from opponent." << std::endl;
            } else {
                std::cout << "Invalid choice." << std::endl;
            }
        }}
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