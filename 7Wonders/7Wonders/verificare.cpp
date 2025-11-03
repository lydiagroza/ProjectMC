#include "Board.h"
#include "CardNode.h"
#include <vector>
#include <iostream>

int main() {
    // Creează 20 de cărți
    std::vector<CardNode*> cards;
    for (int i = 1; i <= 20; ++i)
        cards.push_back(new CardNode("C" + std::to_string(i)));

    Board board;
   
    // Construiește piramida pentru era 3
    board.setupCards(3, cards);
    board.printBoard();
	board.printChildrenList();

    // curățare
    for (auto* c : cards)
        delete c;

    return 0;
}
