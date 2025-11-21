#pragma once
#include "Board.h"

class GameSetup
{
private:
	Board m_board;
    std::vector<std::shared_ptr<CardBase>> m_deckAge1;
    std::vector<std::shared_ptr<CardBase>> m_deckAge2;
    std::vector<std::shared_ptr<CardBase>> m_deckAge3;
    std::vector<std::shared_ptr<CardBase>> m_deckGuilds;

    std::vector<std::shared_ptr<ProgressToken>> m_allTokens;

    void loadAllResources();
    void prepareDecks();  
    void prepareTokens();

    std::vector<CardBase*> toRawPointerVector(const std::vector<std::shared_ptr<CardBase>>& sharedDeck);



public:
    GameSetup();

    void startAge(int age);
    Board& getBoard();



};

