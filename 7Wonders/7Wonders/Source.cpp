#include <iostream>
#include <vector>
#include "CardNode.h"
#include "Board.h"

int main() {
    // 1️⃣ Cream 20 de carti hardcodate pentru epoca 3
    std::vector<CardNode*> era3Cards;
    for (int i = 1; i <= 20; ++i) {
        era3Cards.push_back(new CardNode("C" + std::to_string(i)));
    }

    // 2️⃣ Cream Board si setam layout-ul epocii 3
    Board board;

    // 3️⃣ Construim piramida
    board.setupCards(1, era3Cards); // 1 = index epoca

    // 4️⃣ Afisam piramida vizual
    std::cout << "=== Board Vizual ===\n";
    board.printBoard();

    // 5️⃣ Afisam copiii fiecarui nod
   // std::cout << "\n=== Copii Carti ===\n";
    //board.printChildren();

    // 6️⃣ Curatam memoria
    for (auto* card : era3Cards) {
        delete card;
    }

    return 0;
}
