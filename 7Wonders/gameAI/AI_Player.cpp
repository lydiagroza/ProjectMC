#include "AI_Player.h"
#include "Game.h"
#include "Board.h"
#include "CardBase.h"
#include "Wonder.h"
#include "CardNode.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <fstream>

// ... (GameState, Action, QLearningAgent remain exactly as before) ...
// Copying only the relevant part for space, but I will ensure the full file is correct.

// [KEEPING PREVIOUS GameState/Action/QLearningAgent IMPLEMENTATIONS]

std::string GameState::toHash() const {
    std::ostringstream oss;
    int coinBucket = myCoins / 3;
    int militaryBucket = (myMilitaryPosition + 9) / 3;
    int scienceBucket = myScienceSymbols;
    int blueNorm = std::min(myBlueCardCount / 2, 2);
    int redNorm = std::min(myRedCardCount / 2, 2);
    int greenNorm = std::min(myGreenCardCount / 2, 2);
    int yellowNorm = std::min(myYellowCardCount / 2, 2);
    int oppScienceThreat = (oppScienceSymbols >= 4) ? 1 : 0;
    int oppMilitaryThreat = (oppMilitaryPosition >= 6) ? 1 : 0;
    oss << currentAge << "_" << coinBucket << "_" << militaryBucket << "_" 
        << scienceBucket << "_" << blueNorm << "_" << redNorm << "_" 
        << greenNorm << "_" << yellowNorm << "_" << oppScienceThreat << "_" << oppMilitaryThreat;
    return oss.str();
}

GameState GameState::extract(const Player& me, const Player& opponent, const Board& board, int age) {
    GameState state;
    const auto& myResources = me.getResources();
    state.myCoins = myResources.count(Resource::Coin) ? myResources.at(Resource::Coin) : 0;
    state.myWoodCount = myResources.count(Resource::Wood) ? myResources.at(Resource::Wood) : 0;
    state.myClayCount = myResources.count(Resource::Clay) ? myResources.at(Resource::Clay) : 0;
    state.myStoneCount = myResources.count(Resource::Stone) ? myResources.at(Resource::Stone) : 0;
    state.myGlassCount = myResources.count(Resource::Glass) ? myResources.at(Resource::Glass) : 0;
    state.myPapyrusCount = myResources.count(Resource::Papyrus) ? myResources.at(Resource::Papyrus) : 0;
    state.myMilitaryPosition = board.getMilitaryTrack().getPawnPosition();
    state.myScienceSymbols = me.getNrOfScientificSymbols();
    const auto& inventory = me.getInventory();
    state.myBlueCardCount = inventory.count(Color::Blue) ? (int)inventory.at(Color::Blue).size() : 0;
    state.myRedCardCount = inventory.count(Color::Red) ? (int)inventory.at(Color::Red).size() : 0;
    state.myGreenCardCount = inventory.count(Color::Green) ? (int)inventory.at(Color::Green).size() : 0;
    state.myYellowCardCount = inventory.count(Color::Yellow) ? (int)inventory.at(Color::Yellow).size() : 0;
    const auto& oppResources = opponent.getResources();
    state.oppCoins = oppResources.count(Resource::Coin) ? oppResources.at(Resource::Coin) : 0;
    state.oppMilitaryPosition = -state.myMilitaryPosition;
    state.oppScienceSymbols = opponent.getNrOfScientificSymbols();
    state.currentAge = age;
    state.remainingCards = 0;
    const auto& rows = board.getPyramid().getRows();
    for (const auto& row : rows) {
        for (const auto& node : row) {
            if (node && !node->isPlayed()) state.remainingCards++;
        }
    }
    return state;
}

std::string Action::toString() const {
    std::ostringstream oss;
    switch (type) {
    case BUY_CARD: oss << "BUY_" << cardId; break;
    case BUILD_WONDER: oss << "WONDER_" << wonderId << "_CARD_" << cardId; break;
    case DISCARD_CARD: oss << "DISCARD_" << cardId; break;
    }
    return oss.str();
}

QLearningAgent::QLearningAgent() {
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned int>(seed));
}

Action QLearningAgent::selectAction(const GameState& state, const std::vector<Action>& possibleActions, bool explore) {
    if (possibleActions.empty()) throw std::runtime_error("No possible actions!");
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    if (explore && dist(rng) < explorationRate) {
        std::uniform_int_distribution<size_t> indexDist(0, possibleActions.size() - 1);
        return possibleActions[indexDist(rng)];
    }
    std::string stateHash = state.toHash();
    float maxQ = -std::numeric_limits<float>::infinity();
    Action bestAction = possibleActions[0];
    for (const auto& action : possibleActions) {
        float q = getQValue(stateHash, action);
        if (q > maxQ) { maxQ = q; bestAction = action; }
    }
    return bestAction;
}

void QLearningAgent::updateQValue(const GameState& state, const Action& action, float reward, const GameState& nextState, const std::vector<Action>& nextActions) {
    std::string stateHash = state.toHash();
    std::string nextStateHash = nextState.toHash();
    float currentQ = getQValue(stateHash, action);
    float maxNextQ = nextActions.empty() ? 0.0f : getMaxQValue(nextStateHash, nextActions);
    float newQ = currentQ + learningRate * (reward + discountFactor * maxNextQ - currentQ);
    qTable[stateHash][action] = newQ;
    episodeHistory.push_back({ state, action, reward });
}

void QLearningAgent::endEpisode(bool won, int finalScore) {
    float finalReward = won ? 100.0f : -50.0f;
    finalReward += finalScore * 0.5f;
    for (auto& [state, action, reward] : episodeHistory) {
        std::string stateHash = state.toHash();
        float currentQ = getQValue(stateHash, action);
        qTable[stateHash][action] = currentQ + learningRate * finalReward * 0.1f;
    }
    episodeHistory.clear();
    explorationRate *= 0.995f;
    if (explorationRate < 0.01f) explorationRate = 0.01f;
}

float QLearningAgent::getQValue(const std::string& stateHash, const Action& action) const {
    auto stateIt = qTable.find(stateHash);
    if (stateIt == qTable.end()) return 0.0f;
    auto actionIt = stateIt->second.find(action);
    if (actionIt == stateIt->second.end()) return 0.0f;
    return actionIt->second;
}

float QLearningAgent::getMaxQValue(const std::string& stateHash, const std::vector<Action>& actions) const {
    float maxQ = -std::numeric_limits<float>::infinity();
    for (const auto& action : actions) {
        float q = getQValue(stateHash, action);
        maxQ = std::max(maxQ, q);
    }
    return maxQ == -std::numeric_limits<float>::infinity() ? 0.0f : maxQ;
}

void QLearningAgent::saveModel(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    for (const auto& [stateHash, actions] : qTable) {
        for (const auto& [action, qValue] : actions) {
            file << stateHash << "|" << action.toString() << "|" << qValue << "\n";
        }
    }
    file.close();
}

void QLearningAgent::loadModel(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        file.open("../" + filename);
    }
    if (!file.is_open()) {
        file.open("../../" + filename);
    }
    
    if (!file.is_open()) return;
    qTable.clear();
    std::string line;
    while (std::getline(file, line)) {
        size_t pipe1 = line.find('|');
        size_t pipe2 = line.find('|', pipe1 + 1);
        if (pipe1 == std::string::npos || pipe2 == std::string::npos) continue;
        std::string stateHash = line.substr(0, pipe1);
        std::string actionStr = line.substr(pipe1 + 1, pipe2 - pipe1 - 1);
        float qValue = std::stof(line.substr(pipe2 + 1));
        Action action;
        if (actionStr.find("BUY_") == 0) {
            action.type = Action::BUY_CARD;
            action.cardId = static_cast<uint16_t>(std::stoi(actionStr.substr(4)));
            action.wonderId = 0;
        }
        else if (actionStr.find("WONDER_") == 0) {
            action.type = Action::BUILD_WONDER;
            size_t uPos = actionStr.find("_CARD_");
            action.wonderId = static_cast<uint16_t>(std::stoi(actionStr.substr(7, (int)uPos - 7)));
            action.cardId = static_cast<uint16_t>(std::stoi(actionStr.substr(uPos + 6)));
        }
        else if (actionStr.find("DISCARD_") == 0) {
            action.type = Action::DISCARD_CARD;
            action.cardId = static_cast<uint16_t>(std::stoi(actionStr.substr(8)));
            action.wonderId = 0;
        }
        else continue;
        qTable[stateHash][action] = qValue;
    }
    file.close();
}

// ============================================================================ 
// AI_Player Implementation (Refactored)
// ============================================================================ 

AI_Player::AI_Player(const std::string& name, int id, AI_Difficulty difficulty, AI_Strategy strategy)
    : Player(name, id), m_difficulty(difficulty), m_strategy(strategy), m_isTraining(false), m_currentAge(1)
{
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    m_rng.seed(static_cast<unsigned int>(seed));
    if (m_strategy == AI_Strategy::BALANCED && m_difficulty != AI_Difficulty::EASY) {
        std::uniform_int_distribution<int> dist(0, 3);
        int val = dist(m_rng);
        switch (val) {
        case 0: m_strategy = AI_Strategy::MILITARY; break;
        case 1: m_strategy = AI_Strategy::SCIENCE; break;
        case 2: m_strategy = AI_Strategy::ECONOMY; break;
        default: m_strategy = AI_Strategy::BALANCED; break;
        }
        std::cout << "[AI] " << name << " adopted strategy: " << (int)m_strategy << "\n";
    }
    if (m_difficulty == AI_Difficulty::HARD || m_difficulty == AI_Difficulty::ADAPTIVE) {
        m_learningAgent = std::make_unique<QLearningAgent>();
        m_learningAgent->loadModel("ai_model.txt");
    }
}

int AI_Player::evaluateMilitaryValue(const Player& me, const std::shared_ptr<CardBase>& card) {
    int value = 0;
    if (card->getColor() == Color::Red) {
        value += 50;
        if (me.hasExtraMilitary()) value += 20;
    }
    return value;
}

int AI_Player::evaluateScienceValue(const Player& me, const std::shared_ptr<CardBase>& card) {
    int value = 0;
    if (card->getColor() == Color::Green) {
        value += 60;
        int currentSymbols = me.getNrOfScientificSymbols();
        if (currentSymbols >= 4) value += 100;
        else if (currentSymbols >= 2) value += 40;
    }
    return value;
}

int AI_Player::evaluateEconomyValue(const Player& me, const std::shared_ptr<CardBase>& card) {
    int value = 0;
    if (card->getColor() == Color::Yellow) {
        value += 40;
        value += me.getCoins() / 2;
    }
    if (card->getColor() == Color::Brown || card->getColor() == Color::Gray) {
        value += 45;
    }
    return value;
}

int AI_Player::evaluateCardStatic(const Player& me, const std::shared_ptr<CardBase>& card, const Player& opponent, AI_Strategy strategy, AI_Difficulty difficulty) {
    int value = 0;
    switch (strategy) {
    case AI_Strategy::MILITARY:
        value += evaluateMilitaryValue(me, card) * 2;
        value += evaluateEconomyValue(me, card) / 2;
        value += evaluateScienceValue(me, card) / 3;
        break;
    case AI_Strategy::SCIENCE:
        value += evaluateScienceValue(me, card) * 2;
        value += evaluateEconomyValue(me, card) / 2;
        value += evaluateMilitaryValue(me, card) / 3;
        break;
    case AI_Strategy::ECONOMY:
        value += evaluateEconomyValue(me, card) * 2;
        value += evaluateMilitaryValue(me, card) / 2;
        value += evaluateScienceValue(me, card) / 2;
        break;
    case AI_Strategy::BALANCED:
        value += evaluateMilitaryValue(me, card);
        value += evaluateScienceValue(me, card);
        value += evaluateEconomyValue(me, card);
        break;
    }
    if (card->getColor() == Color::Blue) value += 35;
    if (card->getColor() == Color::Purple) {
        value += 60;
        if (strategy == AI_Strategy::ECONOMY) value += 10;
    }
    if (card->getSymbol().has_value()) value += 25;
    uint8_t cost = me.findTotalCost(*card, opponent);
    if (cost == 255) value = -1000;
    else value -= cost * 5;
    return value;
}

int AI_Player::evaluateWonderStatic(const Player& me, const Wonder& wonder, AI_Strategy strategy) {
    int value = 60;
    std::string name = wonder.getName();
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    if (strategy == AI_Strategy::MILITARY) {
        if (name.find("ZEUS") != std::string::npos || name.find("CIRCUS") != std::string::npos || name.find("COLOSSUS") != std::string::npos) value += 50;
    }
    else if (strategy == AI_Strategy::SCIENCE) {
        if (name.find("LIBRARY") != std::string::npos || name.find("TEMPLE") != std::string::npos) value += 50;
    }
    else if (strategy == AI_Strategy::ECONOMY) {
        if (name.find("LIGHTHOUSE") != std::string::npos || name.find("PIRAEUS") != std::string::npos || name.find("GARDENS") != std::string::npos) value += 50;
    }
    value += static_cast<int>(wonder.getEffects().size()) * 20;
    uint8_t rareCost = 0;
    rareCost += wonder.getCostForResource(Resource::Glass);
    rareCost += wonder.getCostForResource(Resource::Papyrus);
    value -= rareCost * 10;
    return value;
}

bool AI_Player::shouldBuyCard(const std::shared_ptr<CardBase>& card, const Player& opponent) {
    uint8_t cost = findTotalCost(*card, opponent);
    if (cost == 255) return false;
    if (m_difficulty == AI_Difficulty::EASY) return getCoins() >= cost;
    int cardValue = evaluateCardStatic(*this, card, opponent, m_strategy, m_difficulty);
    int costThreshold = (m_difficulty == AI_Difficulty::MEDIUM) ? 35 : 55;
    if (m_difficulty >= AI_Difficulty::HARD && getCoins() < 4) costThreshold += 20;
    return cardValue >= costThreshold;
}

bool AI_Player::shouldConstructWonder(const std::shared_ptr<CardBase>& card, const Player& opponent) {
    bool hasUnbuiltWonder = false;
    for (const auto& wonder : getWonders()) {
        if (!wonder->getIsBuilt()) { hasUnbuiltWonder = true; break; }
    }
    if (!hasUnbuiltWonder) return false;
    if (m_difficulty == AI_Difficulty::EASY) {
        std::uniform_int_distribution<int> dist(0, 100);
        return dist(m_rng) < 20;
    }
    if (m_difficulty == AI_Difficulty::MEDIUM) {
        int cardValue = evaluateCardStatic(*this, card, opponent, m_strategy, m_difficulty);
        return cardValue < 40;
    }
    int cardValue = evaluateCardStatic(*this, card, opponent, m_strategy, m_difficulty);
    int wonderValue = 0;
    for (const auto& wonder : getWonders()) {
        if (!wonder->getIsBuilt()) {
            wonderValue = std::max(wonderValue, evaluateWonderStatic(*this, *wonder, m_strategy));
        }
    }
    return wonderValue > cardValue;
}

std::shared_ptr<CardBase> AI_Player::chooseBestCardHeuristic(const std::vector<std::shared_ptr<CardBase>>& availableCards, const Player& opponent, const Board& board) {
    if (availableCards.empty()) return nullptr;
    if (m_difficulty == AI_Difficulty::EASY) {
        std::uniform_int_distribution<size_t> dist(0, availableCards.size() - 1);
        return availableCards[dist(m_rng)];
    }
    std::shared_ptr<CardBase> bestCard = nullptr;
    int bestValue = -10000;
    for (const auto& card : availableCards) {
        int value = evaluateCardStatic(*this, card, opponent, m_strategy, m_difficulty);
        if (value > bestValue) { bestValue = value; bestCard = card; }
    }
    return bestCard;
}

Wonder* AI_Player::chooseBestWonder(const std::vector<Wonder*>& availableWonders) {
    if (availableWonders.empty()) return nullptr;
    if (m_difficulty == AI_Difficulty::EASY) {
        std::uniform_int_distribution<size_t> dist(0, availableWonders.size() - 1);
        return availableWonders[dist(m_rng)];
    }
    Wonder* bestWonder = nullptr;
    int bestValue = -1000;
    for (auto* wonder : availableWonders) {
        int value = evaluateWonderStatic(*this, *wonder, m_strategy);
        if (value > bestValue) { bestValue = value; bestWonder = wonder; }
    }
    return bestWonder;
}

std::vector<std::shared_ptr<CardBase>> AI_Player::getPlayableCards(const Board& board) const {
    std::vector<std::shared_ptr<CardBase>> playableCards;
    const auto& rows = board.getPyramid().getRows();
    for (const auto& row : rows) {
        for (const auto& node : row) {
            if (node && node->getCard() && !node->isPlayed() && node->isPlayable()) {
                playableCards.push_back(node->getCard());
            }
        }
    }
    return playableCards;
}

std::vector<Action> AI_Player::getPossibleActions(const std::vector<std::shared_ptr<CardBase>>& playableCards, const Player& opponent) const {
    std::vector<Action> actions;
    for (const auto& card : playableCards) {
        uint16_t cid = card->getId();
        uint8_t cost = findTotalCost(*card, opponent);
        if (cost != 255) actions.push_back({ Action::BUY_CARD, cid, 0 });
        for (const auto& wonder : getWonders()) {
            if (!wonder->getIsBuilt()) {
                uint8_t wonderCost = findTotalCost(*wonder, opponent);
                if (wonderCost != 255) actions.push_back({ Action::BUILD_WONDER, cid, wonder->getId() });
            }
        }
        actions.push_back({ Action::DISCARD_CARD, cid, 0 });
    }
    return actions;
}

float AI_Player::calculateReward(const GameState& oldState, const GameState& newState, const Action& action) const {
    float reward = 0.0f;
    if (newState.myScienceSymbols > oldState.myScienceSymbols) { reward += 30.0f; if (newState.myScienceSymbols >= 5) reward += 100.0f; }
    if (newState.myMilitaryPosition > oldState.myMilitaryPosition) { reward += 20.0f; if (newState.myMilitaryPosition >= 8) reward += 80.0f; }
    if (newState.myBlueCardCount > oldState.myBlueCardCount) reward += 15.0f;
    if (newState.myCoins > oldState.myCoins) reward += 5.0f;
    if (newState.myCoins < 2) reward -= 10.0f;
    if (action.type == Action::BUILD_WONDER) reward += 40.0f;
    if (action.type == Action::DISCARD_CARD) reward -= 5.0f;
    if (oldState.oppScienceSymbols >= 5 && newState.oppScienceSymbols == oldState.oppScienceSymbols) reward += 50.0f;
    return reward;
}

std::shared_ptr<CardBase> AI_Player::chooseBestCardML(const std::vector<std::shared_ptr<CardBase>>& availableCards, Player& opponent, Board& board) {
    if (availableCards.empty() || !m_learningAgent) return nullptr;
    GameState currentState = GameState::extract(*this, opponent, board, m_currentAge);
    std::vector<Action> possibleActions = getPossibleActions(availableCards, opponent);
    if (possibleActions.empty()) return nullptr;
    Action chosenAction = m_learningAgent->selectAction(currentState, possibleActions, m_isTraining);
    m_lastState = currentState;
    m_lastAction = chosenAction;
    for (const auto& card : availableCards) {
        if (card->getId() == chosenAction.cardId) return card;
    }
    return availableCards[0];
}

void AI_Player::makeDecision(Board& board, Player& opponent, int currentAge) {
    m_currentAge = currentAge;
    std::cout << "\n>>> AI (" << getName() << ") deciding...\n";
    auto playableCards = getPlayableCards(board);
    if (playableCards.empty()) return;
    std::shared_ptr<CardBase> selectedCard = nullptr;
    bool isMLChoice = false;
    if (m_difficulty == AI_Difficulty::HARD || m_difficulty == AI_Difficulty::ADAPTIVE) {
        selectedCard = chooseBestCardML(playableCards, opponent, board);
        if (selectedCard) {
            isMLChoice = true;
            std::cout << "[AI-ML] Selected Action: " << m_lastAction.toString() << "\n";
            Action executedAction = m_lastAction;
            bool actionSuccess = false;
            if (executedAction.type == Action::BUY_CARD) {
                if (buyCard(selectedCard, opponent, board)) {
                    std::cout << "[AI-ML] Buys card " << selectedCard->getName() << "\n";
                    actionSuccess = true;
                }
            }
            else if (executedAction.type == Action::BUILD_WONDER) {
                 Wonder* targetWonder = nullptr;
                 for(const auto& w : getWonders()) {
                     if(w->getId() == executedAction.wonderId) { targetWonder = w.get(); break; }
                 }
                 if (targetWonder && !targetWonder->getIsBuilt()) {
                     if (constructWonder(selectedCard, *targetWonder, opponent, board)) {
                         std::cout << "[AI-ML] Builds Wonder " << targetWonder->getName() << "\n";
                         if (Game::currentGame) Game::currentGame->notifyWonderBuilt();
                         actionSuccess = true;
                     }
                 }
            }
            if (!actionSuccess) {
                 discardCard(selectedCard, board);
                 std::cout << "[AI-ML] Discards card " << selectedCard->getName() << "\n";
                 executedAction.type = Action::DISCARD_CARD; 
            }
            if (m_learningAgent && m_isTraining) {
                GameState newState = GameState::extract(*this, opponent, board, m_currentAge);
                float reward = calculateReward(m_lastState, newState, executedAction);
                auto nextCards = getPlayableCards(board);
                auto nextActions = getPossibleActions(nextCards, opponent);
                m_learningAgent->updateQValue(m_lastState, executedAction, reward, newState, nextActions);
            }
        }
    }
    if (!isMLChoice) {
        selectedCard = chooseBestCardHeuristic(playableCards, opponent, board);
        if (!selectedCard) return;
        std::cout << "[AI] Selected: " << selectedCard->getName() << "\n";
        bool actionTaken = false;
        if (shouldBuyCard(selectedCard, opponent)) {
            if (buyCard(selectedCard, opponent, board)) {
                std::cout << "[AI] Buys card.\n";
                actionTaken = true;
            }
        }
        if (!actionTaken && shouldConstructWonder(selectedCard, opponent)) {
            std::vector<Wonder*> availableWonders;
            for (const auto& wonder : getWonders()) {
                if (!wonder->getIsBuilt()) availableWonders.push_back(const_cast<Wonder*>(wonder.get()));
            }
            if (!availableWonders.empty()) {
                Wonder* chosenWonder = chooseBestWonder(availableWonders);
                if (chosenWonder) {
                    if (constructWonder(selectedCard, *chosenWonder, opponent, board)) {
                        std::cout << "[AI] Builds wonder: " << chosenWonder->getName() << "\n";
                        if (Game::currentGame) Game::currentGame->notifyWonderBuilt();
                        actionTaken = true;
                    }
                }
            }
        }
        if (!actionTaken) {
            discardCard(selectedCard, board);
            std::cout << "[AI] Discards card.\n";
        }
    }
    const auto& rows = board.getPyramid().getRows();
    for (const auto& row : rows) {
        for (const auto& node : row) {
            if (node && node->getCard() && node->getCard()->getId() == selectedCard->getId()) {
                const_cast<CardNode*>(node.get())->updatePlayedStatus(true);
                break;
            }
        }
    }
    board.updateVisibility();
}

std::shared_ptr<Wonder> AI_Player::chooseWonderFromDraft(const std::vector<std::shared_ptr<Wonder>>& availableWonders) {
    if (availableWonders.empty()) return nullptr;
    if (m_difficulty == AI_Difficulty::EASY) {
        std::uniform_int_distribution<size_t> dist(0, availableWonders.size() - 1);
        return availableWonders[dist(m_rng)];
    }
    Wonder* bestWonder = nullptr;
    int bestValue = -1000;
    for (const auto& wonder : availableWonders) {
        int value = evaluateWonderStatic(*this, *wonder, m_strategy);
        if (value > bestValue) { bestValue = value; bestWonder = wonder.get(); }
    }
    if (bestWonder) {
        for(const auto& w : availableWonders) if(w.get() == bestWonder) return w;
    }
    return availableWonders[0]; 
}

std::shared_ptr<ProgressToken> AI_Player::chooseProgressToken(const std::vector<std::shared_ptr<ProgressToken>>& availableTokens) {
    if (availableTokens.empty()) return nullptr;
    if (m_difficulty == AI_Difficulty::EASY) {
        std::uniform_int_distribution<size_t> dist(0, availableTokens.size() - 1);
        return availableTokens[dist(m_rng)];
    }
    std::map<std::string, int> tokenValues = {
        {"Strategy", 60}, {"Philosophy", 60}, {"Theology", 50}, 
        {"Agriculture", 50}, {"Architecture", 50}, {"Economy", 50},
        {"Mathematics", 50}, {"Law", 40}, {"Masonry", 40}, {"Urbanism", 40}
    };
    if (m_strategy == AI_Strategy::MILITARY) tokenValues["Strategy"] += 60;
    else if (m_strategy == AI_Strategy::SCIENCE) { tokenValues["Philosophy"] += 50; tokenValues["Mathematics"] += 50; tokenValues["Law"] += 40; }
    else if (m_strategy == AI_Strategy::ECONOMY) { tokenValues["Agriculture"] += 40; tokenValues["Economy"] += 50; tokenValues["Urbanism"] += 30; }
    std::shared_ptr<ProgressToken> bestToken = availableTokens[0];
    int bestValue = -100;
    for (const auto& token : availableTokens) {
        int val = tokenValues.count(token->getName()) ? tokenValues.at(token->getName()) : 30;
        if (val > bestValue) { bestValue = val; bestToken = token; }
    }
    return bestToken;
}

void AI_Player::onGameEnd(bool won, int finalScore) {
    if (m_learningAgent && m_isTraining) {
        m_learningAgent->endEpisode(won, finalScore);
        static int gameCount = 0;
        if (++gameCount % 10 == 0) {
            m_learningAgent->saveModel("ai_model.txt");
            std::cout << "[AI] Model auto-saved after " << gameCount << " games.\n";
        }
    }
}
void AI_Player::saveModel(const std::string& filename) const { if (m_learningAgent) m_learningAgent->saveModel(filename); }
void AI_Player::loadModel(const std::string& filename) { if (m_learningAgent) m_learningAgent->loadModel(filename); }


// ============================================================================ 
// HINT SYSTEM IMPLEMENTATION (AI POWERED)
// ============================================================================ 

std::string AI_Player::getBestMoveHint(const Player& me, const Player& opponent, const Board& board, int currentAge, int selectedCardId) {
    std::vector<std::shared_ptr<CardBase>> playableCards;
    const auto& rows = board.getPyramid().getRows();
    for (const auto& row : rows) {
        for (const auto& node : row) {
            if (node && node->getCard() && !node->isPlayed() && node->isPlayable()) {
                playableCards.push_back(node->getCard());
            }
        }
    }

    if (playableCards.empty()) return "Hmm... There are no cards to play!";

    // --- STRATEGY DETECTION LOGIC ---
    AI_Strategy currentStrategy = AI_Strategy::BALANCED;
    int redCount = 0;
    int greenCount = me.getNrOfScientificSymbols();
    int yellowCount = 0;
    int resourceCount = 0;
    const auto& inventory = me.getInventory();
    if (inventory.count(Color::Red)) redCount = inventory.at(Color::Red).size();
    if (inventory.count(Color::Yellow)) yellowCount = inventory.at(Color::Yellow).size();
    if (inventory.count(Color::Brown)) resourceCount += inventory.at(Color::Brown).size();
    if (inventory.count(Color::Gray)) resourceCount += inventory.at(Color::Gray).size();
    if (redCount >= 2 && redCount > greenCount) currentStrategy = AI_Strategy::MILITARY;
    else if (greenCount >= 2 && greenCount > redCount) currentStrategy = AI_Strategy::SCIENCE;
    else if (yellowCount >= 2 || (resourceCount >= 4 && me.getCoins() > 6)) currentStrategy = AI_Strategy::ECONOMY;

    // --- ML BRAIN LOGIC ---
    QLearningAgent brain;
    brain.loadModel("ai_model.txt");
    GameState state = GameState::extract(me, opponent, board, currentAge);
    std::string stateHash = state.toHash();

    // Decide if we are evaluating a SPECIFIC card or ALL cards
    std::vector<std::shared_ptr<CardBase>> cardsToEvaluate;
    if (selectedCardId != -1) {
        // Find the selected card
        for (const auto& c : playableCards) {
            if (c->getId() == selectedCardId) {
                cardsToEvaluate.push_back(c);
                break;
            }
        }
        if (cardsToEvaluate.empty()) return "Hmm... (ID: " + std::to_string(selectedCardId) + ") I can't find that card on the board!";
    } else {
        // Evaluate ALL cards
        cardsToEvaluate = playableCards;
    }

    // Collect possible actions for the chosen scope
    std::vector<Action> actionsToScore;
    for (const auto& card : cardsToEvaluate) {
        uint16_t cid = card->getId();
        if (me.findTotalCost(*card, opponent) != 255) actionsToScore.push_back({ Action::BUY_CARD, cid, 0 });
        for (const auto& w : me.getWonders()) {
            if (!w->getIsBuilt() && me.findTotalCost(*w, opponent) != 255) actionsToScore.push_back({ Action::BUILD_WONDER, cid, w->getId() });
        }
        actionsToScore.push_back({ Action::DISCARD_CARD, cid, 0 });
    }

    // --- SCORING ---
    bool hasMLConfidence = false;
    Action bestAction;
    float maxScore = -1000000.0f;
    std::string methodUsed = "Heuristic";

    // 1. Try ML Scoring first
    for (const auto& act : actionsToScore) {
        float q = brain.getQValue(stateHash, act); 
        if (q != 0.0f) {
            hasMLConfidence = true;
            if (q > maxScore) {
                maxScore = q;
                bestAction = act;
            }
        }
    }

    // 2. If ML has no data, fall back to Heuristic Scoring
    if (!hasMLConfidence || maxScore == -1000000.0f) {
        maxScore = -1000000.0f; // Reset
        for (const auto& act : actionsToScore) {
            float score = 0;
            // Find card ptr for heuristic
            std::shared_ptr<CardBase> currentCard = nullptr;
            for(auto c : playableCards) if(c->getId() == act.cardId) { currentCard = c; break; }
            
            if (currentCard) {
                if (act.type == Action::BUY_CARD) {
                    score = evaluateCardStatic(me, currentCard, opponent, currentStrategy, AI_Difficulty::MEDIUM);
                } else if (act.type == Action::BUILD_WONDER) {
                    Wonder* targetW = nullptr;
                    for(const auto& w : me.getWonders()) if(w->getId() == act.wonderId) targetW = w.get();
                    if(targetW) score = evaluateWonderStatic(me, *targetW, currentStrategy);
                } else {
                    score = 10 + me.getCoins()/3; // Discard value
                }
            }

            if (score > maxScore) {
                maxScore = score;
                bestAction = act;
            }
        }
    } else {
        methodUsed = "AI Trained";
    }

    // --- OUTPUT ---
    std::stringstream ss;
    
    ss << "The wise duck of the kingdom mixed up some potions, thought deeply and looked through his magical crystal ball. ";
    ss << "He went to your great palace and said: \n\n";
    ss << "\"Hmm... I think it's wise to ";
    
    // Find card name for display
    std::string cardName = "Unknown Card";
    for(auto c : playableCards) if(c->getId() == bestAction.cardId) cardName = c->getName();

    switch (bestAction.type) {
        case Action::BUY_CARD: 
            ss << "BUILD " << cardName << "!\""; 
            break;
        case Action::BUILD_WONDER: {
            std::string wName = "Wonder";
            for(const auto& w : me.getWonders()) if(w->getId() == bestAction.wonderId) wName = w->getName();
            ss << "use " << cardName << " to construct your majestic wonder, " << wName << "!\"";
            break;
        }
        case Action::DISCARD_CARD: 
            ss << "DISCARD " << cardName << ". The treasury needs more gold!\""; 
            break;
    }

    return ss.str();
}
