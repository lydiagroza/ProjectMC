#pragma once

#include "GameSetup.h"
#include "Player.h"
#include "Board.h"
#include "AI_Player.h" // Needed for AI_Difficulty enum
#include <memory>
#include <optional>
#include <vector>
#include <iostream>
#include "gameExport.h"

static constexpr int MILITARY_VICTORY_THRESHOLD = 9;
static constexpr int SCIENCE_VICTORY_THRESHOLD = 6;
static constexpr int MAX_TOTAL_WONDERS = 7;
static constexpr int COINS_PER_VICTORY_POINT = 3;

class GAME_API Game {
public:
    static Game* currentGame;

private:
    Board m_board;
    GameSetup m_setup;

    std::shared_ptr<Player> m_player1;
    std::shared_ptr<Player> m_player2;

    Player* m_currentPlayer;
    Player* m_opponent;

    int m_currentAge;
    bool m_gameOver;
    int m_numberOfWondersBuilt = 0;

    void printActionMenu() const;
    void draftWondersPhase();

    bool handleWonderConstruction(std::shared_ptr<CardBase> cardUsed);
    void handleProgressTokenChoice();
    void handle7WondersRule();

    // funcții care ajută la determinarea câștigătorului
    int calculatePurpleGuilds(const Player& p) const;
    int getBuiltWondersCount(const Player& player) const;
    int getCategoryCount(const Player& player, Color color) const;

    // era management
    void determinateWhoStartsNextAge();

    // turn management
    void processTurnTransition();
    CardNode* findNodeById(int cardId) const;

    // membri pentru gestionarea fazei de draft
    std::vector<std::shared_ptr<Wonder>> m_draftSet1;
    std::vector<std::shared_ptr<Wonder>> m_draftSet2;
    int m_draftPhase = 0; // 0 = înainte de draft, 1 = draft 1, 2 = draft 2

public:
    Game();
    void initialize();

    // păstrează modificarea colegei (dificultate AI)
    void setPlayerTypes(bool p1IsAI, bool p2IsAI,
        AI_Difficulty difficulty = AI_Difficulty::MEDIUM);

    // păstrează funcția ta
    bool checkForInstantWin();

    int getCurrentAge() const { return m_currentAge; }
    void handlePlayerAction();
    void startNextAge();
    bool isEndOfAge();
    bool isGameOver() const;
    void switchTurn();
    void printGameState() const; // board + players info
    GameSetup& getSetup() { return m_setup; }
    Board& getBoard() { return m_board; }

    void printPlayerInfo(const Player& player,
        std::ostream& os = std::cout) const;

    int calculatePlayerVP(const Player& player) const;

    Player& getPlayer1() { return *m_player1; }
    Player& getPlayer2() { return *m_player2; }

    void handleBuildFromDiscard();

    // funcții pentru faza de draft din UI
    const std::vector<std::shared_ptr<Wonder>>& getCurrentDraftSet() const;
    bool draftWonder(int wonderId);
    Player* getCurrentPlayer();
    Player* getOpponent();
    int getDraftPhase() const;

    std::optional<Player> determinateWinner();
};
