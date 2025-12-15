#pragma once
#include "Wonder.h"
#include "gameExport.h"

class Board;
class CardBase;
class ProgressToken;

class GAME_API GameSetup
{
private:
	Board& m_board;
    std::vector<std::shared_ptr<CardBase>> m_deckAge1;
    std::vector<std::shared_ptr<CardBase>> m_deckAge2;
    std::vector<std::shared_ptr<CardBase>> m_deckAge3;
    std::vector<std::shared_ptr<CardBase>> m_deckGuilds;

    //cartile unique pointer -> si restul aplicatiei ar face referile la ele cu * <- mai ok asa
    //mai usor shared pointer?

    std::vector<std::shared_ptr<ProgressToken>> m_allTokens;

    std::vector<std::shared_ptr<Wonder>> m_allWonders;

    void loadAllResources();
    void prepareDecks();  
    void prepareTokens();


public:
    GameSetup(Board& board);

    GameSetup& startAge(int age);
    Board& getBoard();
    void printDecks()const;
	void printWonders() const;

    std::vector<std::shared_ptr<Wonder>> drawWonders(int count);
};

