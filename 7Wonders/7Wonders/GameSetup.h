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

    std::vector<std::shared_ptr<ProgressToken>> m_allTokens;

    std::vector<std::shared_ptr<Wonder>> m_allWonders;

    void loadAllResources();
    void prepareDecks();  
    void prepareTokens();

    std::vector<CardBase*> toRawPointerVector(const std::vector<std::shared_ptr<CardBase>>& sharedDeck);



public:
    GameSetup(Board& board);

    void startAge(int age);
    Board& getBoard();
    void printDecks()const;
	void printWonders() const;
};

