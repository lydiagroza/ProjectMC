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
    void handle7WondersRule();

    // Helper functions for determining the winner
    int calculatePurpleGuilds(const Player& p) const;
    int getBuiltWondersCount(const Player& player) const;
    int getCategoryCount(const Player& player, Color color) const;

    // Age management
    void determinateWhoStartsNextAge();

    // Turn management
    void processTurnTransition();
    CardNode* findNodeById(int cardId) const;

    // Draft phase management
    std::vector<std::shared_ptr<Wonder>> m_draftSet1;
    std::vector<std::shared_ptr<Wonder>> m_draftSet2;
    int m_draftPhase = 0; // 0 = pre-draft, 1 = draft 1, 2 = draft 2
    bool m_isWaitingForDiscardChoice = false;
    bool m_isWaitingForProgressTokenChoice = false;
    bool m_isWaitingForGreatLibraryChoice = false;
    bool m_isWaitingForOpponentCardDiscard = false;

    Player* m_targetOpponent = nullptr;

    std::vector<std::shared_ptr<CardBase>> m_buildFromDiscardChoices;
    std::vector<std::shared_ptr<CardBase>> m_opponentCardDiscardChoices;

public:
    Game();
   
    void initialize();

    void setPlayerTypes(bool p1IsAI, bool p2IsAI,
        AI_Difficulty difficulty = AI_Difficulty::MEDIUM);

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
    void handleProgressTokenChoice();
    void handleGreatLibraryChoice(); // NEW
    void handleDiscardOpponentCardChoice(Player& opponent, Color cardColor);
    void notifyWonderBuilt(); // Notification for 7 Wonders Rule
    int getNumberOfWondersBuilt() const { return m_numberOfWondersBuilt; }

    // functions for the draft phase from the UI
    const std::vector<std::shared_ptr<Wonder>>& getCurrentDraftSet() const;
    bool draftWonder(int wonderId);
    Player* getCurrentPlayer();
    Player* getOpponent();
    int getDraftPhase() const;

    std::optional<Player> determinateWinner();

    const std::vector<std::shared_ptr<CardBase>>& getDiscardedCards() const;

    bool isWaitingForDiscardChoice() const;
    bool isWaitingForProgressTokenChoice() const;
    bool isWaitingForGreatLibraryChoice() const; // NEW
    bool isWaitingForOpponentCardDiscard() const;

    const std::vector<std::shared_ptr<CardBase>>& getBuildFromDiscardChoices() const;
    const std::vector<std::shared_ptr<CardBase>>& getOpponentCardDiscardChoices() const;

    void resolveBuildFromDiscard(int chosenCardId);
    void resolveProgressTokenChoice(int tokenId);
    void resolveGreatLibraryChoice(int tokenId); // NEW
    void resolveDiscardOpponentCard(int chosenCardId);

    friend class SaveManager;
};
