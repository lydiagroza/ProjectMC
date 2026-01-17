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

    // Getteri pentru serializare
    const std::vector<std::shared_ptr<CardBase>>& getAllAge1Cards() const { return m_deckAge1; }
    const std::vector<std::shared_ptr<CardBase>>& getAllAge2Cards() const { return m_deckAge2; }
    const std::vector<std::shared_ptr<CardBase>>& getAllAge3Cards() const { return m_deckAge3; }
    const std::vector<std::shared_ptr<CardBase>>& getAllGuilds() const { return m_deckGuilds; }
    const std::vector<std::shared_ptr<ProgressToken>>& getAllTokens() const { return m_allTokens; }
    const std::vector<std::shared_ptr<Wonder>>& getAllWonders() const { return m_allWonders; }
};

