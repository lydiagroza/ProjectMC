#pragma once
#include <Player.h>
#include "../GameBase/gameExport.h"
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
    EASY,    // Random decisions
    MEDIUM,  // Heuristic strategy
    HARD,
    ADAPTIVE // Trains in real time
};

enum class AI_Strategy {
    MILITARY,
    SCIENCE,
    ECONOMY,
    BALANCED
};

// Game state for ML
struct GameState {
    // Own resources
    int myCoins;
    int myWoodCount;
    int myClayCount;
    int myStoneCount;
    int myGlassCount;
    int myPapyrusCount;

    // Points and progress
    int myMilitaryPosition;
    int myScienceSymbols;
    int myBlueCardCount;
    int myRedCardCount;
    int myGreenCardCount;
    int myYellowCardCount;

    // Opponent resources
    int oppCoins;
    int oppMilitaryPosition;
    int oppScienceSymbols;

    // Game state
    int currentAge;
    int remainingCards;

    // Hash for Q-Table
    std::string toHash() const;

    // Extract state from game
    static GameState extract(const Player& me, const Player& opponent, const Board& board, int age);
};

// Possible action
struct Action {
    enum Type { BUY_CARD, BUILD_WONDER, DISCARD_CARD };

    Type type;
    uint16_t cardId;
    uint16_t wonderId; // only for BUILD_WONDER

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

    // Select optimal action (or explore)
    Action selectAction(const GameState& state,
                        const std::vector<Action>& possibleActions,
                        bool explore = true);

    // Update Q-value after an action
    void updateQValue(const GameState& state,
                      const Action& action,
                      float reward,
                      const GameState& nextState,
                      const std::vector<Action>& nextActions);

    // After game ends
    void endEpisode(bool won, int finalScore);

    // Persistence
    void saveModel(const std::string& filename) const;
    void loadModel(const std::string& filename);

    // Parameter adjustment
    void setLearningRate(float lr) { learningRate = lr; }
    void setExplorationRate(float er) { explorationRate = er; }

    float getQValue(const std::string& stateHash, const Action& action) const;

private:
    float getMaxQValue(const std::string& stateHash,
                       const std::vector<Action>& actions) const;
};

class GAME_API AI_Player : public Player {
private:
    AI_Difficulty m_difficulty;
    AI_Strategy m_strategy;
    std::mt19937 m_rng;

    // ML Agent (only for HARD and ADAPTIVE)
    std::unique_ptr<QLearningAgent> m_learningAgent;

    // Tracking for training
    GameState m_lastState;
    Action m_lastAction;
    bool m_isTraining;
    int m_currentAge; // Tracked pentru GameState

    // HEURISTIC evaluation functions (for MEDIUM) - STATIC to be used by the Hint System
    static int evaluateCardStatic(const Player& me, const std::shared_ptr<CardBase>& card, const Player& opponent, AI_Strategy strategy = AI_Strategy::BALANCED, AI_Difficulty difficulty = AI_Difficulty::MEDIUM);
    static int evaluateWonderStatic(const Player& me, const Wonder& wonder, AI_Strategy strategy = AI_Strategy::BALANCED);
    
    // Helper functions
    static int evaluateMilitaryValue(const Player& me, const std::shared_ptr<CardBase>& card);
    static int evaluateScienceValue(const Player& me, const std::shared_ptr<CardBase>& card);
    static int evaluateEconomyValue(const Player& me, const std::shared_ptr<CardBase>& card);

    // HEURISTIC decision functions
    std::shared_ptr<CardBase> chooseBestCardHeuristic(
        const std::vector<std::shared_ptr<CardBase>>& availableCards,
        const Player& opponent,
        const Board& board);

    // ML decision functions
    std::shared_ptr<CardBase> chooseBestCardML(
        const std::vector<std::shared_ptr<CardBase>>& availableCards,
        Player& opponent,
        Board& board);

    Wonder* chooseBestWonder(const std::vector<Wonder*>& availableWonders);

    // Helper functions
    bool shouldBuyCard(const std::shared_ptr<CardBase>& card, const Player& opponent);
    bool shouldConstructWonder(const std::shared_ptr<CardBase>& card, const Player& opponent);
    std::vector<std::shared_ptr<CardBase>> getPlayableCards(const Board& board) const;

    // Conversion between cards and actions
    std::vector<Action> getPossibleActions(
        const std::vector<std::shared_ptr<CardBase>>& playableCards,
        const Player& opponent) const;

    // Calculate reward
    float calculateReward(const GameState& oldState,
                          const GameState& newState,
                          const Action& action) const;

public:
    AI_Player(const std::string& name,
              int id,
              AI_Difficulty difficulty = AI_Difficulty::MEDIUM,
              AI_Strategy strategy = AI_Strategy::BALANCED);

    // Main decision function
    void makeDecision(Board& board, Player& opponent, int currentAge);

    // Hint System
    static std::string getBestMoveHint(const Player& me, const Player& opponent, const Board& board, int currentAge, int selectedCardId = -1);

    // Draft wonders
    std::shared_ptr<Wonder> chooseWonderFromDraft(
        const std::vector<std::shared_ptr<Wonder>>& availableWonders);

    // Choose progress token
    std::shared_ptr<ProgressToken> chooseProgressToken(
        const std::vector<std::shared_ptr<ProgressToken>>& availableTokens);

    // Game end notification (for training)
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