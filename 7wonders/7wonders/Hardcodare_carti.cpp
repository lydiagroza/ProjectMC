#include <iostream>
#include <vector>
#include "ProductionCards.h"

using namespace std;

vector<ProductionCard> hardcodeAge1Deck() {
    vector<ProductionCard> deck;

    deck.emplace_back("LUMBER YARD", 1, {}, { {Resource::Wood, 1} });
    deck.emplace_back("LOGGING CAMP", 1, { {Resource::Coin, 1} }, { {Resource::Wood, 1} });
    deck.emplace_back("CLAY POOL", 1, {}, { {Resource::Clay, 1} });
    deck.emplace_back("CLAY PIT", 1, { {Resource::Coin, 1} }, { {Resource::Clay, 1} });
    deck.emplace_back("QUARRY", 1, {}, { {Resource::Stone, 1} });
    deck.emplace_back("STONE PIT", 1, { {Resource::Coin, 1} }, { {Resource::Stone, 1} });
    deck.emplace_back("GLASSWORKS", 1, {}, { {Resource::Glass, 1} });
    deck.emplace_back("PRESS", 1, {}, { {Resource::Papyrus, 1} });
    deck.emplace_back("STONE RESERVE", 1, { {Resource::Coin, 3} }, { {Resource::Stone, 1} });
    deck.emplace_back("CLAY RESERVE", 1, { {Resource::Coin, 3} }, { {Resource::Clay, 1} });
    deck.emplace_back("WOOD RESERVE", 1, { {Resource::Coin, 3} }, { {Resource::Wood, 1} });

    return deck;
}

int main() {
    auto deck = hardcodeAge1Deck();

    cout << "=== AGE I CARDS ===\n";
    for (const auto& card : deck)
        card.display();

    return 0;
}
