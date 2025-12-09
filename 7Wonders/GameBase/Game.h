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

    void switchTurn();
    void handlePlayerAction(); 
    void checkForInstantWin(const MilitaryResult& militaryResult);
    void startNextAge();
    bool isEndOfAge();
	void printGameState() const; //board + players info
    void printActionMenu() const;

	//calculateFinalScore(); -> maybe sa intoarca playerul care a castigat

public:
    Game();
    void initialize();
    GameSetup& getSetup() { return m_setup; }
    Board& getBoard() { return m_board; }
	void printPlayerInfo(const Player& player, std::ostream& os = std::cout) const;
    Player* getCurrentPlayer() const { return m_currentPlayer; }
};
