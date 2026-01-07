#pragma once
#include "GameSetup.h"
#include "Player.h"
#include "Board.h"
#include <memory>
#include <optional>
#include "gameExport.h"

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
	int calculatePlayerVP(const Player& player) const;
	std::optional<Player> determinateWinner();

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

    Player& getPlayer1() { return m_player1; }
    Player& getPlayer2() { return m_player2; }
};
