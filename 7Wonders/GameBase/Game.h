#pragma once
#include "GameSetup.h"
#include "Player.h"
#include "Board.h"
#include <memory>
#include <optional>
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
   

    Player m_player1;
    Player m_player2;

    Player* m_currentPlayer;
    Player* m_opponent;

    int m_currentAge;
    bool m_gameOver;
    int m_numberOfWondersBuilt=0;
   
    bool checkForInstantWin();
    void printActionMenu() const;
    void draftWondersPhase();

    bool handleWonderConstruction(std::shared_ptr<CardBase> cardUsed);
	void handleProgressTokenChoice();
    void handle7WondersRule();

    //fct care ma ajuta sa vad cine a castigat
	
	std::optional<Player> determinateWinner();
	int calculatePurpleGuilds(const Player& p) const;
    int getBuiltWondersCount(const Player& player) const;
	int getCategoryCount(const Player& player, Color color) const;

	//era management
	void determinateWhoStartsNextAge();

	//turn management
    void processTurnTransition();
    CardNode* findNodeById(int cardId) const;


public:
    Game();
    void initialize();
    void handlePlayerAction();
    void startNextAge();
    bool isEndOfAge();
    bool isGameOver() const;
    void switchTurn();
    void printGameState() const; //board + players info
    GameSetup& getSetup() { return m_setup; }
    Board& getBoard() { return m_board; }
	void printPlayerInfo(const Player& player, std::ostream& os = std::cout) const;
    Player* getCurrentPlayer() const { return m_currentPlayer; }
    int calculatePlayerVP(const Player& player) const;

    Player& getPlayer1() { return m_player1; }
    Player& getPlayer2() { return m_player2; }
    void handleBuildFromDiscard();
};
