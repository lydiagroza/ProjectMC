#pragma once
#include "GameSetup.h"
#include "Player.h"
#include "Board.h"
#include <memory>
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
   
    bool checkForInstantWin();
    void printActionMenu() const;
    void draftWondersPhase();

	//calculateFinalScore(); -> maybe sa intoarca playerul care a castigat

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
