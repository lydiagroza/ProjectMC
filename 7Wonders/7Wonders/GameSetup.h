#pragma once
#include "Board.h"
#include "Wonder.h"

class GameSetup
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

    std::vector<CardBase*> toRawPointerVector(const std::vector<std::shared_ptr<CardBase>>& sharedDeck);



public:
    GameSetup(Board& board);

    GameSetup& startAge(int age);
    Board& getBoard();
    void printDecks()const;
	void printWonders() const;
};

