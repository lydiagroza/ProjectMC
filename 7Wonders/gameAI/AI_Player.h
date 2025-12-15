#pragma once
#include "Player.h"
#include "Board.h"
#include "CardBase.h"
#include "Wonder.h"
#include <memory>
#include <vector>
#include <random>

enum class AI_Difficulty {
    EASY,    // Decizii random
    MEDIUM,  // Strategie ok
    HARD     // Strategie buna 
};

enum class AI_Strategy {
    MILITARY,   // Focusează pe cărți militare
    SCIENCE,    // Focusează pe simboluri științifice
    ECONOMY,    // Focusează pe bani și cărți galbene
    BALANCED    // Strategie echilibrată
};

class AI_Player : public  Player {
private:
    AI_Difficulty m_difficulty;
    AI_Strategy m_strategy;
    std::mt19937 m_rng;

    // Funcții de evaluare
    int evaluateCard(const std::shared_ptr<CardBase>& card, const Player& opponent) const;
    int evaluateWonder(const Wonder& wonder) const;
    int evaluateMilitaryValue(const std::shared_ptr<CardBase>& card) const;
    int evaluateScienceValue(const std::shared_ptr<CardBase>& card) const;
    int evaluateEconomyValue(const std::shared_ptr<CardBase>& card) const;

    // Funcții de decizie
    std::shared_ptr<CardBase> chooseBestCard(const std::vector<std::shared_ptr<CardBase>>& availableCards,
        const Player& opponent,
        const Board& board);

    Wonder* chooseBestWonder(const std::vector<Wonder*>& availableWonders) const;

    // Funcții helper
    bool shouldBuyCard(const std::shared_ptr<CardBase>& card, const Player& opponent) const;
    bool shouldConstructWonder(const std::shared_ptr<CardBase>& card, const Player& opponent) const;
    std::vector<std::shared_ptr<CardBase>> getPlayableCards(const Board& board, const Player& opponent) const;

public:
    AI_Player(const std::string& name, AI_Difficulty difficulty = AI_Difficulty::MEDIUM,
        AI_Strategy strategy = AI_Strategy::BALANCED);

    // Funcția principală de decizie AI
    void makeDecision(Board& board, Player& opponent);

    // Funcții pentru draft-ul de wonders
    std::shared_ptr<Wonder> chooseWonderFromDraft(const std::vector<std::shared_ptr<Wonder>>& availableWonders);

    // Getters
    AI_Difficulty getDifficulty() const { return m_difficulty; }
    AI_Strategy getStrategy() const { return m_strategy; }

    // Setters
    void setDifficulty(AI_Difficulty difficulty) { m_difficulty = difficulty; }
    void setStrategy(AI_Strategy strategy) { m_strategy = strategy; }
};