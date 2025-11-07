#include "GameSetup.h"
#include "Board.h"
#include "CardNode.h"
#include <vector>
#include <iostream>


GameSetup::GameSetup()
{
    auto selected = selectProgressTokens();

    // Le dăm Board-ului
    board.setVisibleTokens(selected);
}


std::vector<ProgressToken> selectProgressTokens()
{

    // Copiem toți tokenii
    std::vector<ProgressToken> tokens = allTokens;

    // RNG
    std::random_device rd;
    std::mt19937 gen(rd());

    // Shuffle
    std::shuffle(tokens.begin(), tokens.end(), gen);

    // Primele 5
    return { tokens.begin(), tokens.begin() + 5 };
}

Board& getBoard()
{
    return board;
}
