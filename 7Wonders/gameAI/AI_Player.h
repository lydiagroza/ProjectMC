#pragma once
#include "Player.h"
#include "Board.h"
#include "CardBase.h"
#include "Wonder.h"
#include <memory>
#include <vector>
#include <random>
#include <map>
#include <string>
#include <fstream>

enum class AI_Difficulty {
    EASY,    // Decizii random
    MEDIUM,  // Strategie heuristic
    HARD,    // Q-Learning (ML)
    ADAPTIVE // Se antrenează în timp real
};

enum class AI_Strategy {
    MILITARY,
    SCIENCE,
    ECONOMY,
    BALANCED
};

//
// starea jocului 
struct GameState {
    // Resurse proprii
    int myCoins;
    int myWoodCount;
    int myClayCount;
    int myStoneCount;
    int myGlassCount;
    int myPapyrusCount;

    // Puncte și progres
    int myMilitaryPosition;
    int myScienceSymbols;
    int myBlueCardCount;
    int myRedCardCount;
    int myGreenCardCount;
    int myYellowCardCount;

    // Resurse oponent
    int oppCoins;
    int oppMilitaryPosition;
    int oppScienceSymbols;

    // Starea jocului
    int currentAge;
    int remainingCards;

    // Hde acțiunea
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
//}ash pentru Q-Table
    std::string toHash() const;

    // Extrage starea din joc
    // static GameState extract(const Player& me, const Player& opponent, const Board& board);
};

// Acțiune posibilă
struct Action {
    enum Type { BUY_CARD, BUILD_WONDER, DISCARD_CARD };

    Type type;
    uint16_t cardId;
    uint16_t wonderId; // doar pentru BUILD_WONDER

    std::string toString() const;

    bool operator<(const Action& other) const {
        if (type != other.type) return type < other.type;
        if (cardId != other.cardId) return cardId < other.cardId;
        return wonderId < other.wonderId;
    }
};

// Q-Learning Brain
class QLearningAgent {
private:
    // Q-Table: State -> Action -> Value
    std::map<std::string, std::map<Action, float>> qTable;

    // Hyperparameters
    float learningRate = 0.1f;      // Alpha
    float discountFactor = 0.95f;   // Gamma
    float explorationRate = 0.2f;   // Epsilon

    // Training data
    std::vector<std::tuple<GameState, Action, float>> episodeHistory;

    std::mt19937 rng;

public:
    QLearningAgent();

    // Alege acțiunea optimă (sau explorează)
    Action selectAction(const GameState& state,
        const std::vector<Action>& possibleActions,
        bool explore = true);

    // Update Q-value după o acțiune
    void updateQValue(const GameState& state,
        const Action& action,
        float reward,
        const GameState& nextState,
        const std::vector<Action>& nextActions);

    // După terminarea jocului
    void endEpisode(bool won, int finalScore);

    // Persistență
    void saveModel(const std::string& filename) const;
    void loadModel(const std::string& filename);

    // Ajustare parametri
    void setLearningRate(float lr) { learningRate = lr; }
    void setExplorationRate(float er) { explorationRate = er; }

private:
    float getQValue(const std::string& stateHash, const Action& action) const;
    float getMaxQValue(const std::string& stateHash,
        const std::vector<Action>& actions) const;
};



class AI_Player : public Player {
private:
    AI_Difficulty m_difficulty;
    AI_Strategy m_strategy;
    std::mt19937 m_rng;

    // ML Agent (doar pentru HARD și ADAPTIVE)
    std::unique_ptr<QLearningAgent> m_learningAgent;

    // Tracking pentru antrenament
    GameState m_lastState;
    Action m_lastAction;
    bool m_isTraining;

    // Funcții de evaluare HEURISTIC (pentru MEDIUM)
    int evaluateCard(const std::shared_ptr<CardBase>& card, const Player& opponent) const;
    int evaluateWonder(const Wonder& wonder) const;
    int evaluateMilitaryValue(const std::shared_ptr<CardBase>& card) const;
    int evaluateScienceValue(const std::shared_ptr<CardBase>& card) const;
    int evaluateEconomyValue(const std::shared_ptr<CardBase>& card) const;

    // Funcții de decizie HEURISTIC
    std::shared_ptr<CardBase> chooseBestCardHeuristic(
        const std::vector<std::shared_ptr<CardBase>>& availableCards,
        const Player& opponent,
        const Board& board);

    // Funcții de decizie ML
    std::shared_ptr<CardBase> chooseBestCardML(
        const std::vector<std::shared_ptr<CardBase>>& availableCards,
        const Player& opponent,
        const Board& board);

    Wonder* chooseBestWonder(const std::vector<Wonder*>& availableWonders) ;

    // Funcții helper
    bool shouldBuyCard(const std::shared_ptr<CardBase>& card, const Player& opponent) ;
    bool shouldConstructWonder(const std::shared_ptr<CardBase>& card, const Player& opponent) ;
    std::vector<std::shared_ptr<CardBase>> getPlayableCards(const Board& board, const Player& opponent) const;

    // Conversie între cărți și acțiuni
    std::vector<Action> getPossibleActions(
        const std::vector<std::shared_ptr<CardBase>>& playableCards,
        const Player& opponent) const;

    // Calculează reward
    float calculateReward(const GameState& oldState,
        const GameState& newState,
        const Action& action) const;

public:
    AI_Player(const std::string& name,
        int id,
        AI_Difficulty difficulty = AI_Difficulty::MEDIUM,
        AI_Strategy strategy = AI_Strategy::BALANCED);

    // Funcția principală de decizie
    void makeDecision(Board& board, Player& opponent);

    // Draft wonders
    std::shared_ptr<Wonder> chooseWonderFromDraft(
        const std::vector<std::shared_ptr<Wonder>>& availableWonders);

    // Notificare sfârșitul jocului (pentru training)
    void onGameEnd(bool won, int finalScore);

    // Training mode
    void enableTraining(bool enable = true) { m_isTraining = enable; }
    bool isTraining() const { return m_isTraining; }

    // Model management
    void saveModel(const std::string& filename) const;
    void loadModel(const std::string& filename);

    // Getters
    AI_Difficulty getDifficulty() const { return m_difficulty; }
    AI_Strategy getStrategy() const { return m_strategy; }

    // Setters
    void setDifficulty(AI_Difficulty difficulty) { m_difficulty = difficulty; }
    void setStrategy(AI_Strategy strategy) { m_strategy = strategy; }
};
