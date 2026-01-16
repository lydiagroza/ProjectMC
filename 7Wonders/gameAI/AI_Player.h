#pragma once
#include "Player.h"
#include <memory>
#include <vector>
#include <random>
#include <map>
#include <string>
#include <cstdint>

// Forward declarations
class Player;
class Board;
class CardBase;
class Wonder;

enum class AI_Difficulty {
    EASY,    // Decizii random
    MEDIUM,  // Strategie heuristică
    HARD,    // Q-Learning (ML)
    ADAPTIVE // Se antrenează în timp real
};

enum class AI_Strategy {
    MILITARY,
    SCIENCE,
    ECONOMY,
    BALANCED
};

// Starea jocului pentru ML
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

    // Hash pentru Q-Table
    std::string toHash() const;

    // Extrage starea din joc
    static GameState extract(const Player& me, const Player& opponent, const Board& board, int age);
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
    int m_currentAge; // Tracked pentru GameState

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
        Player& opponent,
        Board& board);

    Wonder* chooseBestWonder(const std::vector<Wonder*>& availableWonders);

    // Funcții helper
    bool shouldBuyCard(const std::shared_ptr<CardBase>& card, const Player& opponent);
    bool shouldConstructWonder(const std::shared_ptr<CardBase>& card, const Player& opponent);
    std::vector<std::shared_ptr<CardBase>> getPlayableCards(const Board& board) const;

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
    void makeDecision(Board& board, Player& opponent, int currentAge);

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