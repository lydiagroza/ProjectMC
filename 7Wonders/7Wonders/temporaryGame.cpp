#include "GameSetup.h"
#include <iostream>

int main()
{
    GameSetup setup;            // tot setup-ul se face automat
    Board& board = setup.getBoard();

    board.printTokens();        // token-urile vizibile
    board.printCardsTree();     // piramida de cărți
    board.printChildrenList();

    return 0;
}

