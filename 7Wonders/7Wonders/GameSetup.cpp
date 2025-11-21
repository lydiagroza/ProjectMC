#include "GameSetup.h"
#include <algorithm>
#include <random>

GameSetup::GameSetup()
{
    
    // Selectăm 5 token-uri random
    auto selected = this->selectProgressTokens();
    // Le dăm Board-ului
    board.setAvailableProgressTokens(selected);

    auto eraCards = SelectRandomCards();
    board.setupCards(3, eraCards);
}

std::vector<ProgressToken> GameSetup::selectProgressTokens()
{
    // Copiem toți tokenii
    std::vector<ProgressToken> tokens = m_allTokens;

    // RNG
    std::random_device rd;
    std::mt19937 gen(rd());

    // Shuffle
    std::shuffle(tokens.begin(), tokens.end(), gen);

    // Returnăm primele 5 token-uri
    return { tokens.begin(), tokens.begin() + 5 };
}

std::vector<CardNode*> GameSetup::SelectRandomCards()
{
    // Copiem vectorul original ca să nu-l modificăm
    std::vector<CardNode*> cards = m_cards;  // allCards = vectorul tău de 23 de cărți

    // RNG
    std::random_device rd;
    std::mt19937 gen(rd());

    // Shuffle
    std::shuffle(cards.begin(), cards.end(), gen);

    // Returnăm primele 20
    return { cards.begin(), cards.begin() + 20 };
}




Board& GameSetup::getBoard()
{
    return board;
}
