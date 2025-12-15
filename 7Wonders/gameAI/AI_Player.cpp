//cpp gameAI\AI_Player.cpp
//#include "AI_Player.h"
//#include <algorithm>
//#include <chrono>
//#include <iostream>
//
//AI_Player::AI_Player(const std::string& name, AI_Difficulty difficulty, AI_Strategy strategy)
//    : Player(name), m_difficulty(difficulty), m_strategy(strategy)
//{
//    // Inițializare generator random
//    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
//    m_rng.seed(static_cast<unsigned int>(seed));
//}
//
//std::vector<std::shared_ptr<CardBase>> AI_Player::getPlayableCards(const Board& board, const Player& opponent) const {
//    std::vector<std::shared_ptr<CardBase>> playableCards;
//
//    const auto& rows = board.getPyramid().getRows();
//    for (const auto& row : rows) {
//        for (const auto& node : row) {
//            if (node && node->getCard() && !node->isPlayed() && node->isPlayable()) {
//                playableCards.push_back(node->getCard());
//            }
//        }
//    }
//
//    return playableCards;
//}
//
//int AI_Player::evaluateMilitaryValue(const std::shared_ptr<CardBase>& card) const {
//    int value = 0;
//
//    if (card->getColor() == Color::Red) {
//        value += 50; // Bază pentru cărți militare
//        // Estimare puncte militare (ar trebui să accesăm efectele, simplificat aici)
//        value += 20; // Bonus estimat pentru puncte militare
//    }
//
//    return value;
//}
//
//int AI_Player::evaluateScienceValue(const std::shared_ptr<CardBase>& card) const {
//    int value = 0;
//
//    if (card->getColor() == Color::Green) {
//        value += 60; // Bază pentru cărți științifice
//
//        // Bonus dacă completează un set
//        int currentSymbols = getNrOfScientificSymbols();
//        if (currentSymbols >= 4) {
//            value += 100; // Mari șanse de victorie științifică
//        }
//    }
//
//    return value;
//}
//
//int AI_Player::evaluateEconomyValue(const std::shared_ptr<CardBase>& card) const {
//    int value = 0;
//
//    if (card->getColor() == Color::Yellow) {
//        value += 40; // Bază pentru cărți comerciale
//        value += getCoins() / 2; // Bonus bazat pe bani existenți
//    }
//
//    // Cărți maro/gri (resurse)
//    if (card->getColor() == Color::Brown || card->getColor() == Color::Gray) {
//        value += 30;
//    }
//
//    return value;
//}
//
//int AI_Player::evaluateCard(const std::shared_ptr<CardBase>& card, const Player& opponent) const {
//    int value = 0;
//
//    // Evaluare bazată pe strategie
//    switch (m_strategy) {
//    case AI_Strategy::MILITARY:
//        value += evaluateMilitaryValue(card) * 2;
//        value += evaluateEconomyValue(card) / 2;
//        value += evaluateScienceValue(card) / 3;
//        break;
//
//    case AI_Strategy::SCIENCE:
//        value += evaluateScienceValue(card) * 2;
//        value += evaluateEconomyValue(card) / 2;
//        value += evaluateMilitaryValue(card) / 3;
//        break;
//
//    case AI_Strategy::ECONOMY:
//        value += evaluateEconomyValue(card) * 2;
//        value += evaluateMilitaryValue(card) / 2;
//        value += evaluateScienceValue(card) / 2;
//        break;
//
//    case AI_Strategy::BALANCED:
//        value += evaluateMilitaryValue(card);
//        value += evaluateScienceValue(card);
//        value += evaluateEconomyValue(card);
//        break;
//    }
//
//    // Bonus pentru cărți blue (VP)
//    if (card->getColor() == Color::Blue) {
//        value += 35;
//    }
//
//    // Bonus pentru chain symbols
//    if (card->getSymbol().has_value()) {
//        value += 25;
//    }
//
//    // Penalizare dacă cartea e scumpă
//    uint8_t cost = findTotalCost(*card, opponent);
//    if (cost == 255) { // Nu poate cumpăra
//        value = -1000;
//    }
//    else {
//        value -= cost * 5; // Penalizare proporțională cu costul
//    }
//
//    return value;
//}
//
//int AI_Player::evaluateWonder(const Wonder& wonder) const {
//    int value = 50; // Valoare de bază
//
//    // Bonus pentru efecte specifice (simplificat)
//    const auto& effects = wonder.getEffects();
//    value += effects.size() * 30;
//
//    return value;
//}
//
//bool AI_Player::shouldBuyCard(const std::shared_ptr<CardBase>& card, const Player& opponent) const {
//    uint8_t cost = findTotalCost(*card, opponent);
//
//    if (cost == 255) return false; // Nu poate cumpăra
//
//    // Easy: Cumpără dacă are bani
//    if (m_difficulty == AI_Difficulty::EASY) {
//        return getCoins() >= cost;
//    }
//
//    // Medium/Hard: Evaluează valoarea
//    int cardValue = evaluateCard(card, opponent);
//    int costThreshold = (m_difficulty == AI_Difficulty::MEDIUM) ? 30 : 50;
//
//    return cardValue > costThreshold;
//}
//
//bool AI_Player::shouldConstructWonder(const std::shared_ptr<CardBase>& card, const Player& opponent) const {
//    // Verifică dacă are wonders neconstruite
//    bool hasUnbuiltWonder = false;
//    for (const auto& wonder : getWonders()) {
//        if (!wonder.getIsBuilt()) {
//            hasUnbuiltWonder = true;
//            break;
//        }
//    }
//
//    if (!hasUnbuiltWonder) return false;
//
//    // Easy: Rareori construiește wonders
//    if (m_difficulty == AI_Difficulty::EASY) {
//        std::uniform_int_distribution<int> dist(0, 100);
//        return dist(const_cast<std::mt19937&>(m_rng)) < 20; // 20% șansă
//    }
//
//    // Medium: Construiește dacă cartea nu e foarte valoroasă
//    if (m_difficulty == AI_Difficulty::MEDIUM) {
//        int cardValue = evaluateCard(card, opponent);
//        return cardValue < 40;
//    }
//
//    // Hard: Strategie complexă
//    int cardValue = evaluateCard(card, opponent);
//    int wonderValue = 0;
//
//    for (const auto& wonder : getWonders()) {
//        if (!wonder.getIsBuilt()) {
//            wonderValue = std::max(wonderValue, evaluateWonder(wonder));
//        }
//    }
//
//    return wonderValue > cardValue;
//}
//
//std::shared_ptr<CardBase> AI_Player::chooseBestCard(const std::vector<std::shared_ptr<CardBase>>& availableCards,
//    const Player& opponent,
//    const Board& board) {
//    if (availableCards.empty()) return nullptr;
//
//    // Easy: Alege random
//    if (m_difficulty == AI_Difficulty::EASY) {
//        std::uniform_int_distribution<size_t> dist(0, availableCards.size() - 1);
//        return availableCards[dist(m_rng)];
//    }
//
//    // Medium/Hard: Evaluează și alege cel mai bun
//    std::shared_ptr<CardBase> bestCard = nullptr;
//    int bestValue = -10000;
//
//    for (const auto& card : availableCards) {
//        int value = evaluateCard(card, opponent);
//
//        if (value > bestValue) {
//            bestValue = value;
//            bestCard = card;
//        }
//    }
//
//    return bestCard;
//}
//
//Wonder* AI_Player::chooseBestWonder(const std::vector<Wonder*>& availableWonders) const {
//    if (availableWonders.empty()) return nullptr;
//
//    // Easy: Alege random
//    if (m_difficulty == AI_Difficulty::EASY) {
//        std::uniform_int_distribution<size_t> dist(0, availableWonders.size() - 1);
//        return availableWonders[dist(const_cast<std::mt19937&>(m_rng))];
//    }
//
//    // Medium/Hard: Evaluează
//    Wonder* bestWonder = nullptr;
//    int bestValue = -1000;
//
//    for (auto* wonder : availableWonders) {
//        int value = evaluateWonder(*wonder);
//
//        if (value > bestValue) {
//            bestValue = value;
//            bestWonder = wonder;
//        }
//    }
//
//    return bestWonder;
//}
//
//void AI_Player::makeDecision(Board& board, Player& opponent) {
//    std::cout << "\n>>> AI (" << getName() << ") ia decizia...\n";
//
//    // Obține cărțile jucabile
//    auto playableCards = getPlayableCards(board, opponent);
//
//    if (playableCards.empty()) {
//        std::cout << "AI: Nu sunt cărți disponibile.\n";
//        return;
//    }
//
//    // Alege cea mai bună carte
//    auto selectedCard = chooseBestCard(playableCards, opponent, board);
//
//    if (!selectedCard) {
//        std::cout << "AI: Eroare la alegerea cărții.\n";
//        return;
//    }
//
//    std::cout << "AI alege: " << selectedCard->getName() << "\n";
//
//    // Decide acțiunea
//    bool actionTaken = false;
//
//    // 1. Încearcă să cumpere cartea
//    if (shouldBuyCard(selectedCard, opponent)) {
//        if (buyCard(selectedCard, opponent, board)) {
//            std::cout << "AI cumpără cartea.\n";
//            actionTaken = true;
//        }
//    }
//
//    // 2. Încearcă să construiască wonder
//    if (!actionTaken && shouldConstructWonder(selectedCard, opponent)) {
//        std::vector<Wonder*> availableWonders;
//        for (const auto& wonder : getWonders()) {
//            if (!wonder.getIsBuilt()) {
//                availableWonders.push_back(const_cast<Wonder*>(&wonder));
//            }
//        }
//
//        if (!availableWonders.empty()) {
//            Wonder* chosenWonder = chooseBestWonder(availableWonders);
//            if (chosenWonder) {
//                constructWonder(selectedCard, *chosenWonder, opponent, board);
//                std::cout << "AI construiește minunea: " << chosenWonder->getName() << "\n";
//                actionTaken = true;
//            }
//        }
//    }
//
//    // 3. Fallback: Vinde cartea
//    if (!actionTaken) {
//        discardCard(*selectedCard);
//        std::cout << "AI vinde cartea pentru bani.\n";
//    }
//
//    // Actualizează board
//    // Marchează cartea ca jucată
//    const auto& rows = board.getPyramid().getRows();
//    for (const auto& row : rows) {
//        for (const auto& node : row) {
//            if (node && node->getCard() && node->getCard()->getId() == selectedCard->getId()) {
//                const_cast<CardNode*>(node.get())->updatePlayedStatus(true);
//                break;
//            }
//        }
//    }
//
//    board.updateVisibility();
//}
//
//std::shared_ptr<Wonder> AI_Player::chooseWonderFromDraft(const std::vector<std::shared_ptr<Wonder>>& availableWonders) {
//    if (availableWonders.empty()) return nullptr;
//
//    std::cout << "AI (" << getName() << ") alege o minune...\n";
//
//    // Easy: Random
//    if (m_difficulty == AI_Difficulty::EASY) {
//        std::uniform_int_distribution<size_t> dist(0, availableWonders.size() - 1);
//        auto chosen = availableWonders[dist(m_rng)];
//        std::cout << "AI alege: " << chosen->getName() << "\n";
//        return chosen;
//    }
//
//    // Medium/Hard: Evaluează
//    std::shared_ptr<Wonder> bestWonder = nullptr;
//    int bestValue = -1000;
//
//    for (const auto& wonder : availableWonders) {
//        int value = evaluateWonder(*wonder);
//
//        if (value > bestValue) {
//            bestValue = value;
//            bestWonder = wonder;
//        }
//    }
//
//    if (bestWonder) {
//        std::cout << "AI alege: " << bestWonder->getName() << "\n";
//    }
//
//    return bestWonder;
//}