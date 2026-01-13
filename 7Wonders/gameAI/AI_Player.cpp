#include "AI_Player.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>

std::string GameState::toHash() const {
    std::ostringstream oss;

    // Normalizăm valorile pentru a reduce spațiul de stări
    int coinBucket = myCoins / 3;           // 0-2, 3-5, 6-8, etc.
    int militaryBucket = (myMilitaryPosition + 9) / 3; // -9..9 -> 0..6
    int scienceBucket = myScienceSymbols;   // 0-6 (exact)

    // Normalizăm numărul de cărți (0, 1-2, 3-4, 5+)
    int blueNorm = std::min(myBlueCardCount / 2, 2);
    int redNorm = std::min(myRedCardCount / 2, 2);
    int greenNorm = std::min(myGreenCardCount / 2, 2);
    int yellowNorm = std::min(myYellowCardCount / 2, 2);

    // Info critică despre oponent
    int oppScienceThreat = (oppScienceSymbols >= 4) ? 1 : 0;
    int oppMilitaryThreat = (oppMilitaryPosition >= 6) ? 1 : 0;


    oss << currentAge << "_"
        << coinBucket << "_"
        << militaryBucket << "_"
        << scienceBucket << "_"
        << blueNorm << "_"
        << redNorm << "_"
        << greenNorm << "_"
        << yellowNorm << "_"
        << oppScienceThreat << "_"
        << oppMilitaryThreat;

    return oss.str();
}

GameState GameState::extract(const Player& me, const Player& opponent, const Board& board, int age) {
    GameState state;

    // Resurse proprii
    const auto& myResources = me.getResources();
    state.myCoins = myResources.count(Resource::Coin) ? myResources.at(Resource::Coin) : 0;
    state.myWoodCount = myResources.count(Resource::Wood) ? myResources.at(Resource::Wood) : 0;
    state.myClayCount = myResources.count(Resource::Clay) ? myResources.at(Resource::Clay) : 0;
    state.myStoneCount = myResources.count(Resource::Stone) ? myResources.at(Resource::Stone) : 0;
    state.myGlassCount = myResources.count(Resource::Glass) ? myResources.at(Resource::Glass) : 0;
    state.myPapyrusCount = myResources.count(Resource::Papyrus) ? myResources.at(Resource::Papyrus) : 0;

    // Progres militar și științific
    state.myMilitaryPosition = board.getMilitaryTrack().getPawnPosition();
    state.myScienceSymbols = me.getNrOfScientificSymbols();

    // Inventar
    const auto& inventory = me.getInventory();
    state.myBlueCardCount = inventory.count(Color::Blue) ? inventory.at(Color::Blue).size() : 0;
    state.myRedCardCount = inventory.count(Color::Red) ? inventory.at(Color::Red).size() : 0;
    state.myGreenCardCount = inventory.count(Color::Green) ? inventory.at(Color::Green).size() : 0;
    state.myYellowCardCount = inventory.count(Color::Yellow) ? inventory.at(Color::Yellow).size() : 0;

    // Info despre oponent
    const auto& oppResources = opponent.getResources();
    state.oppCoins = oppResources.count(Resource::Coin) ? oppResources.at(Resource::Coin) : 0;
    state.oppMilitaryPosition = -state.myMilitaryPosition; // Poziție relativă
    state.oppScienceSymbols = opponent.getNrOfScientificSymbols();

    // Starea jocului
    state.currentAge = age;
    state.remainingCards = 0;

    const auto& rows = board.getPyramid().getRows();
    for (const auto& row : rows) {
        for (const auto& node : row) {
            if (node && !node->isPlayed()) {
                state.remainingCards++;
            }
        }
    }

    return state;
}



std::string Action::toString() const {
    std::ostringstream oss;
    switch (type) {
    case BUY_CARD:
        oss << "BUY_" << cardId;
        break;
    case BUILD_WONDER:
        oss << "WONDER_" << wonderId << "_CARD_" << cardId;
        break;
    case DISCARD_CARD:
        oss << "DISCARD_" << cardId;
        break;
    }
    return oss.str();
}



QLearningAgent::QLearningAgent() {
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned int>(seed));
}

Action QLearningAgent::selectAction(const GameState& state,
    const std::vector<Action>& possibleActions,
    bool explore) {
    if (possibleActions.empty()) {
        throw std::runtime_error("No possible actions!");
    }

    // Epsilon-greedy exploration
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    if (explore && dist(rng) < explorationRate) {
        // EXPLORARE: Alege random
        std::uniform_int_distribution<size_t> indexDist(0, possibleActions.size() - 1);
        return possibleActions[indexDist(rng)];
    }

    // EXPLOATARE: Alege acțiunea cu cel mai mare Q-value
    std::string stateHash = state.toHash();
    float maxQ = -std::numeric_limits<float>::infinity();
    Action bestAction = possibleActions[0];

    for (const auto& action : possibleActions) {
        float q = getQValue(stateHash, action);
        if (q > maxQ) {
            maxQ = q;
            bestAction = action;
        }
    }

    return bestAction;
}

void QLearningAgent::updateQValue(const GameState& state,
    const Action& action,
    float reward,
    const GameState& nextState,
    const std::vector<Action>& nextActions) {
    std::string stateHash = state.toHash();
    std::string nextStateHash = nextState.toHash();

    float currentQ = getQValue(stateHash, action);
    float maxNextQ = nextActions.empty() ? 0.0f : getMaxQValue(nextStateHash, nextActions);

    // Q-Learning formula: Q(s,a) = Q(s,a) + α[r + γ*max(Q(s',a')) - Q(s,a)]
    float newQ = currentQ + learningRate * (reward + discountFactor * maxNextQ - currentQ);

    qTable[stateHash][action] = newQ;

    // Salvăm pentru episod
    episodeHistory.push_back({ state, action, reward });
}

void QLearningAgent::endEpisode(bool won, int finalScore) {
    // Reward final
    float finalReward = won ? 100.0f : -50.0f;
    finalReward += finalScore * 0.5f;

    // Backpropagate reward prin tot episodul
    for (auto& [state, action, reward] : episodeHistory) {
        std::string stateHash = state.toHash();
        float currentQ = getQValue(stateHash, action);
        qTable[stateHash][action] = currentQ + learningRate * finalReward * 0.1f;
    }

    episodeHistory.clear();

    // Decay exploration rate
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

float QLearningAgent::getMaxQValue(const std::string& stateHash,
    const std::vector<Action>& actions) const {
    float maxQ = -std::numeric_limits<float>::infinity();
    for (const auto& action : actions) {
        float q = getQValue(stateHash, action);
        maxQ = std::max(maxQ, q);
    }
    return maxQ == -std::numeric_limits<float>::infinity() ? 0.0f : maxQ;
}

void QLearningAgent::saveModel(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to save model to " << filename << std::endl;
        return;
    }

    for (const auto& [stateHash, actions] : qTable) {
        for (const auto& [action, qValue] : actions) {
            file << stateHash << "|"
                << action.toString() << "|"
                << qValue << "\n";
        }
    }

    file.close();
    std::cout << "[AI] Model saved: " << qTable.size() << " states, "
        << "exploration rate: " << explorationRate << "\n";
}

void QLearningAgent::loadModel(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "[AI] No existing model found. Starting fresh.\n";
        return;
    }

    qTable.clear();
    std::string line;
    int loadedEntries = 0;

    while (std::getline(file, line)) {
        size_t pipe1 = line.find('|');
        size_t pipe2 = line.find('|', pipe1 + 1);

        if (pipe1 == std::string::npos || pipe2 == std::string::npos) continue;

        std::string stateHash = line.substr(0, pipe1);
        std::string actionStr = line.substr(pipe1 + 1, pipe2 - pipe1 - 1);
        float qValue = std::stof(line.substr(pipe2 + 1));

        // Parse action string: "BUY_15" / "WONDER_3_CARD_20" / "DISCARD_8"
        Action action;
        if (actionStr.find("BUY_") == 0) {
            action.type = Action::BUY_CARD;
            action.cardId = static_cast<uint16_t>(std::stoi(actionStr.substr(4)));
            action.wonderId = 0;
        }
        else if (actionStr.find("WONDER_") == 0) {
            action.type = Action::BUILD_WONDER;
            size_t underscorePos = actionStr.find("_CARD_");
            action.wonderId = static_cast<uint16_t>(std::stoi(actionStr.substr(7, underscorePos - 7)));
            action.cardId = static_cast<uint16_t>(std::stoi(actionStr.substr(underscorePos + 6)));
        }
        else if (actionStr.find("DISCARD_") == 0) {
            action.type = Action::DISCARD_CARD;
            action.cardId = static_cast<uint16_t>(std::stoi(actionStr.substr(8)));
            action.wonderId = 0;
        }
        else {
            continue; // Invalid action string
        }

        qTable[stateHash][action] = qValue;
        loadedEntries++;
    }

    file.close();
    std::cout << "[AI] Model loaded: " << loadedEntries << " entries.\n";
}



AI_Player::AI_Player(const std::string& name, int id,
    AI_Difficulty difficulty, AI_Strategy strategy)
    : Player(name, id),
    m_difficulty(difficulty),
    m_strategy(strategy),
    m_isTraining(false),
    m_currentAge(1)
{
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    m_rng.seed(static_cast<unsigned int>(seed));

    // Creează agent doar pentru HARD/ADAPTIVE
    if (m_difficulty == AI_Difficulty::HARD || m_difficulty == AI_Difficulty::ADAPTIVE) {
        m_learningAgent = std::make_unique<QLearningAgent>();
        m_learningAgent->loadModel("ai_model.txt");
    }
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

std::vector<Action> AI_Player::getPossibleActions(
    const std::vector<std::shared_ptr<CardBase>>& playableCards,
    const Player& opponent) const {
    std::vector<Action> actions;

    for (const auto& card : playableCards) {
        uint16_t cid = card->getId();

        // Opțiunea 1: Cumpără carte
        uint8_t cost = findTotalCost(*card, opponent);
        if (cost != 255) { // Poate cumpăra
            actions.push_back({ Action::BUY_CARD, cid, 0 });
        }

        // Opțiunea 2: Construiește wonders
        for (const auto& wonder : getWonders()) {
            if (!wonder->getIsBuilt()) {
                uint8_t wonderCost = findTotalCost(*wonder, opponent);
                if (wonderCost != 255) {
                    actions.push_back({ Action::BUILD_WONDER, cid, wonder->getId() });
                }
            }
        }

        // Opțiunea 3: Vinde carte (mereu posibil)
        actions.push_back({ Action::DISCARD_CARD, cid, 0 });
    }

    return actions;
}

float AI_Player::calculateReward(const GameState& oldState,
    const GameState& newState,
    const Action& action) const {
    float reward = 0.0f;

    // Reward pentru progres științific
    if (newState.myScienceSymbols > oldState.myScienceSymbols) {
        reward += 30.0f;
        if (newState.myScienceSymbols >= 5) reward += 100.0f; // Aproape de victorie!
    }

    // Reward pentru progres militar
    if (newState.myMilitaryPosition > oldState.myMilitaryPosition) {
        reward += 20.0f;
        if (newState.myMilitaryPosition >= 8) reward += 80.0f;
    }

    // Reward pentru cărți blue (VP)
    if (newState.myBlueCardCount > oldState.myBlueCardCount) {
        reward += 15.0f;
    }

    // Reward pentru economie
    if (newState.myCoins > oldState.myCoins) {
        reward += 5.0f;
    }

    // Penalizare pentru sărăcie
    if (newState.myCoins < 2) {
        reward -= 10.0f;
    }

    // Bonus pentru construirea wonders
    if (action.type == Action::BUILD_WONDER) {
        reward += 40.0f;
    }

    // Penalizare pentru discard (nu e optim)
    if (action.type == Action::DISCARD_CARD) {
        reward -= 5.0f;
    }

    // Urgență defensivă
    if (oldState.oppScienceSymbols >= 5) {
        // Dacă AI a blocat progresul oponentului
        if (newState.oppScienceSymbols == oldState.oppScienceSymbols) {
            reward += 50.0f;
        }
    }

    return reward;
}

// ============================================================================
// HEURISTIC EVALUATION (pentru MEDIUM)
// ============================================================================

int AI_Player::evaluateMilitaryValue(const std::shared_ptr<CardBase>& card) const {
    int value = 0;
    if (card->getColor() == Color::Red) {
        value += 50;
        // Bonus dacă avem extra military
        if (hasExtraMilitary()) {
            value += 20;
        }
    }
    return value;
}

int AI_Player::evaluateScienceValue(const std::shared_ptr<CardBase>& card) const {
    int value = 0;
    if (card->getColor() == Color::Green) {
        value += 60;
        int currentSymbols = getNrOfScientificSymbols();
        if (currentSymbols >= 4) value += 100; // Aproape de victorie!
        else if (currentSymbols >= 2) value += 40;
    }
    return value;
}

int AI_Player::evaluateEconomyValue(const std::shared_ptr<CardBase>& card) const {
    int value = 0;
    if (card->getColor() == Color::Yellow) {
        value += 40;
        value += getCoins() / 2;
    }
    if (card->getColor() == Color::Brown || card->getColor() == Color::Gray) {
        value += 30;
    }
    return value;
}

int AI_Player::evaluateCard(const std::shared_ptr<CardBase>& card, const Player& opponent) const {
    int value = 0;

    switch (m_strategy) {
    case AI_Strategy::MILITARY:
        value += evaluateMilitaryValue(card) * 2;
        value += evaluateEconomyValue(card) / 2;
        value += evaluateScienceValue(card) / 3;
        break;
    case AI_Strategy::SCIENCE:
        value += evaluateScienceValue(card) * 2;
        value += evaluateEconomyValue(card) / 2;
        value += evaluateMilitaryValue(card) / 3;
        break;
    case AI_Strategy::ECONOMY:
        value += evaluateEconomyValue(card) * 2;
        value += evaluateMilitaryValue(card) / 2;
        value += evaluateScienceValue(card) / 2;
        break;
    case AI_Strategy::BALANCED:
        value += evaluateMilitaryValue(card);
        value += evaluateScienceValue(card);
        value += evaluateEconomyValue(card);
        break;
    }

    // Bonus pentru cărți blue (VP)
    if (card->getColor() == Color::Blue) value += 35;

    // Bonus pentru chain symbols
    if (card->getSymbol().has_value()) value += 25;

    // Penalizare pentru cost
    uint8_t cost = findTotalCost(*card, opponent);
    if (cost == 255) {
        value = -1000; // Nu putem cumpăra
    }
    else {
        value -= cost * 5;
    }

    return value;
}

int AI_Player::evaluateWonder(const Wonder& wonder) const {
    int value = 50;
    const auto& effects = wonder.getEffects();
    value += static_cast<int>(effects.size()) * 30;

    uint8_t totalCost = 0;
    for (const auto& [res, amount] : wonder.getCost()) {
        if (res == Resource::Coin) continue;
        totalCost += amount;
    }
    if (totalCost <= 3) value += 20;

    return value;
}

bool AI_Player::shouldBuyCard(const std::shared_ptr<CardBase>& card, const Player& opponent) {
    uint8_t cost = findTotalCost(*card, opponent);
    if (cost == 255) return false;

    if (m_difficulty == AI_Difficulty::EASY) {
        return getCoins() >= cost;
    }

    int cardValue = evaluateCard(card, opponent);
    int costThreshold = (m_difficulty == AI_Difficulty::MEDIUM) ? 30 : 50;
    return cardValue > costThreshold;
}

bool AI_Player::shouldConstructWonder(const std::shared_ptr<CardBase>& card, const Player& opponent) {
    bool hasUnbuiltWonder = false;
    for (const auto& wonder : getWonders()) {
        if (!wonder->getIsBuilt()) {
            hasUnbuiltWonder = true;
            break;
        }
    }
    if (!hasUnbuiltWonder) return false;

    if (m_difficulty == AI_Difficulty::EASY) {
        std::uniform_int_distribution<int> dist(0, 100);
        return dist(m_rng) < 20; // 20% șansă
    }

    if (m_difficulty == AI_Difficulty::MEDIUM) {
        int cardValue = evaluateCard(card, opponent);
        return cardValue < 40; // Dacă cartea nu e grozavă, construiește wonder
    }

    // HARD: Compară valori
    int cardValue = evaluateCard(card, opponent);
    int wonderValue = 0;
    for (const auto& wonder : getWonders()) {
        if (!wonder->getIsBuilt()) {
            wonderValue = std::max(wonderValue, evaluateWonder(*wonder));
        }
    }
    return wonderValue > cardValue;
}

std::shared_ptr<CardBase> AI_Player::chooseBestCardHeuristic(
    const std::vector<std::shared_ptr<CardBase>>& availableCards,
    const Player& opponent,
    const Board& board) {
    if (availableCards.empty()) return nullptr;

    if (m_difficulty == AI_Difficulty::EASY) {
        std::uniform_int_distribution<size_t> dist(0, availableCards.size() - 1);
        return availableCards[dist(m_rng)];
    }

    std::shared_ptr<CardBase> bestCard = nullptr;
    int bestValue = -10000;

    for (const auto& card : availableCards) {
        int value = evaluateCard(card, opponent);
        if (value > bestValue) {
            bestValue = value;
            bestCard = card;
        }
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
        int value = evaluateWonder(*wonder);
        if (value > bestValue) {
            bestValue = value;
            bestWonder = wonder;
        }
    }

    return bestWonder;
}

// ============================================================================
// ML DECISION MAKING
// ============================================================================

std::shared_ptr<CardBase> AI_Player::chooseBestCardML(
    const std::vector<std::shared_ptr<CardBase>>& availableCards,
    Player& opponent,
    Board& board) {
    if (availableCards.empty() || !m_learningAgent) return nullptr;

    // Extrage starea
    GameState currentState = GameState::extract(*this, opponent, board, m_currentAge);

    // Obține acțiunile posibile
    std::vector<Action> possibleActions = getPossibleActions(availableCards, opponent);
    if (possibleActions.empty()) return nullptr;

    // Selectează acțiunea
    Action chosenAction = m_learningAgent->selectAction(currentState, possibleActions, m_isTraining);

    // Salvează pentru update ulterior
    m_lastState = currentState;
    m_lastAction = chosenAction;

    // Găsește cartea corespunzătoare
    for (const auto& card : availableCards) {
        if (card->getId() == chosenAction.cardId) {
            return card;
        }
    }

    return availableCards[0]; // Fallback
}

// ============================================================================
// MAIN DECISION FUNCTION
// ============================================================================

void AI_Player::makeDecision(Board& board, Player& opponent, int currentAge) {
    m_currentAge = currentAge;
    std::cout << "\n>>> AI (" << getName() << ") deciding...\n";

    auto playableCards = getPlayableCards(board);
    if (playableCards.empty()) {
        std::cout << "[AI] No cards available.\n";
        return;
    }

    // Alege carte
    std::shared_ptr<CardBase> selectedCard;
    if (m_difficulty == AI_Difficulty::HARD || m_difficulty == AI_Difficulty::ADAPTIVE) {
        selectedCard = chooseBestCardML(playableCards, opponent, board);
    }
    else {
        selectedCard = chooseBestCardHeuristic(playableCards, opponent, board);
    }

    if (!selectedCard) {
        std::cout << "[AI] Error choosing card.\n";
        return;
    }

    std::cout << "[AI] Selected: " << selectedCard->getName() << "\n";

    // Decide acțiunea
    bool actionTaken = false;
    Action executedAction;

    // 1. Încearcă să cumpere cartea
    if (shouldBuyCard(selectedCard, opponent)) {
        if (buyCard(selectedCard, opponent, board)) {
            std::cout << "[AI] Buys card.\n";
            executedAction = { Action::BUY_CARD, selectedCard->getId(), 0 };
            actionTaken = true;
        }
    }

    // 2. Încearcă să construiască wonder
    if (!actionTaken && shouldConstructWonder(selectedCard, opponent)) {
        std::vector<Wonder*> availableWonders;
        for (const auto& wonder : getWonders()) {
            if (!wonder->getIsBuilt()) {
                availableWonders.push_back(const_cast<Wonder*>(wonder.get()));
            }
        }

        if (!availableWonders.empty()) {
            Wonder* chosenWonder = chooseBestWonder(availableWonders);
            if (chosenWonder) {
                constructWonder(selectedCard, *chosenWonder, opponent, board);
                std::cout << "[AI] Builds wonder: " << chosenWonder->getName() << "\n";
                executedAction = { Action::BUILD_WONDER, selectedCard->getId(), chosenWonder->getId() };
                actionTaken = true;
            }
        }
    }

    // 3. Fallback: Vinde cartea
    if (!actionTaken) {
        discardCard(*selectedCard);
        std::cout << "[AI] Discards card.\n";
        executedAction = { Action::DISCARD_CARD, selectedCard->getId(), 0 };
    }

    // ML: Update Q-value
    if (m_learningAgent && m_isTraining) {
        GameState newState = GameState::extract(*this, opponent, board, m_currentAge);
        float reward = calculateReward(m_lastState, newState, executedAction);

        auto nextCards = getPlayableCards(board);
        auto nextActions = getPossibleActions(nextCards, opponent);

        m_learningAgent->updateQValue(m_lastState, executedAction, reward, newState, nextActions);
    }

    // Update board - marchează cartea ca jucată
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

std::shared_ptr<Wonder> AI_Player::chooseWonderFromDraft(
    const std::vector<std::shared_ptr<Wonder>>& availableWonders) {
    if (availableWonders.empty()) return nullptr;

    std::cout << "[AI] (" << getName() << ") choosing wonder...\n";

    if (m_difficulty == AI_Difficulty::EASY) {
        std::uniform_int_distribution<size_t> dist(0, availableWonders.size() - 1);
        auto chosen = availableWonders[dist(m_rng)];
        std::cout << "[AI] Chooses: " << chosen->getName() << "\n";
        return chosen;
    }

    std::shared_ptr<Wonder> bestWonder = nullptr;
    int bestValue = -1000;

    for (const auto& wonder : availableWonders) {
        int value = evaluateWonder(*wonder);
        if (value > bestValue) {
            bestValue = value;
            bestWonder = wonder;
        }
    }

    if (bestWonder) {
        std::cout << "[AI] Chooses: " << bestWonder->getName() << "\n";
    }

    return bestWonder;
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

void AI_Player::saveModel(const std::string& filename) const {
    if (m_learningAgent) {
        m_learningAgent->saveModel(filename);
    }
}

void AI_Player::loadModel(const std::string& filename) {
    if (m_learningAgent) {
        m_learningAgent->loadModel(filename);
    }
}