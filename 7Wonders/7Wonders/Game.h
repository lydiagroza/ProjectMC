#pragma once
#include "GameSetup.h"
#include "Player.h"
#include "Board.h"
#include <memory>

class Game {
private:

    GameSetup m_setup;
    Board m_board;

    Player m_player1;
    Player m_player2;

    Player* m_currentPlayer;
    Player* m_opponent;

    int m_currentAge;
    bool m_gameOver;

    void switchTurn();
    void handlePlayerAction(); 
    void checkForInstantWin(); 
    void startNextAge();
    bool isEndOfAge();
    void printGameState() const;
    void printActionMenu() const;

	//calculateFinalScore(); -> maybe sa intoarca playerul care a castigat

public:
    Game();
	void run();
    GameSetup& getSetup() { return m_setup; }
};
