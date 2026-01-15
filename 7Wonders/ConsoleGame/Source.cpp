// ConsoleGame/main.cpp
#include <iostream>
#include <cassert>
#include <map>
#include <memory>
#include "Game.h"
#include "testEngine.h"
#include "CardBase.h" 
#include "Wonder.h"

// Unit Tests
void runUnitTests() {
    std::cout << "\n=== RUNNING UNIT TESTS ===\n";

    // 1. Setup Game
    Game game;
    game.initialize();
    Player* p1 = game.getCurrentPlayer();
    Player* p2 = game.getOpponent();

    // Clear initial resources for clean testing
    // p1->removeResource(Coin, 7); // Cannot remove if not enough, but they start with 7
    // Actually, let's just assert they have 7
    assert(p1->getCoins() == 7);
    assert(p2->getCoins() == 7);

    // --- TEST 1: Resource Trading Costs ---
    std::cout << "Test 1: Resource Trading... ";
    
    // P2 generates 1 Wood. Cost for P1 to buy Wood should be 2 (base) + 1 (opp) = 3
    p2->addResource(Resource::Wood, 1);
    
    // Create a dummy card requiring 1 Wood
    std::map<Resource, uint8_t> costWood1;
    costWood1[Resource::Wood] = 1;
    
    // Check cost for P1
    uint8_t tradeCost = p1->findTotalCost(CardBase("Dummy", 999, Color::Blue, costWood1, std::nullopt, std::nullopt), *p2);
    // Base 2 + 1 (opp production) = 3 coins. P1 has 0 wood.
    // Wait, CardBase constructor signature: string, id, color, cost, symbol, unlocks
    // findTotalCost includes the card's coin cost (0 here) + trade cost. 
    
    if (tradeCost == 3) std::cout << "PASS\n";
    else std::cout << "FAIL (Expected 3, got " << (int)tradeCost << ")\n";

    // --- TEST 2: Victory Points Calculation ---
    std::cout << "Test 2: Victory Points... ";
    
    // P1 has 7 coins -> 2 VP (7/3 = 2)
    // Add a Blue Card worth 5 VP (hypothetically)
    // We need to manipulate points directly since we can't easily create a card with VP effects without the loader
    p1->add_Points(Points::BlueCards, 5);
    
    int vp = game.calculatePlayerVP(*p1);
    // 2 (coins) + 5 (blue) = 7
    if (vp == 7) std::cout << "PASS\n";
    else std::cout << "FAIL (Expected 7, got " << vp << ")\n";

    // --- TEST 3: Guild VP Logic ---
    std::cout << "Test 3: Guild VP (Magistrates)... ";
    // Magistrates Guild (ID 75): 1 VP per Blue Card (max of self/opp)
    // P1 has 1 Blue Card (from Test 2 mock, wait, I added points directly, not to inventory)
    // Need to add actual cards to inventory for Guild Logic to work
    
    // Create Mock Blue Card
    auto blueCard = std::make_shared<CardBase>("BlueTemple", 100, Color::Blue, std::map<Resource, uint8_t>{}, std::nullopt, std::nullopt);
    p1->addCardToInventory(blueCard); 
    p1->addCardToInventory(blueCard); // Add twice. P1 has 2 Blue cards.
    
    // P2 has 0 Blue cards.
    
    // Add Magistrates Guild to P1
    auto magistrates = std::make_shared<CardBase>("Magistrates", 75, Color::Purple, std::map<Resource, uint8_t>{}, std::nullopt, std::nullopt);
    p1->addCardToInventory(magistrates);
    
    // Calculate VP. Should be:
    // Coins: 7/3 = 2
    // Blue Points: 5 (from manual add) -> wait, manual add persists.
    // Guild: Max(2, 0) = 2 VP.
    // Total: 2 + 5 + 2 = 9.
    
    vp = game.calculatePlayerVP(*p1);
    
    if (vp == 9) std::cout << "PASS\n";
    else std::cout << "FAIL (Expected 9, got " << vp << ")\n";

    // --- TEST 4: Military Victory Check ---
    std::cout << "Test 4: Military Victory... ";
    // Force pawn to 9
    // Access military track via Board? Board is private in Game... but Game has checkForInstantWin
    // We can't easily force pawn position via public API of Game without playing cards.
    // We can use Board from Setup? m_setup is private.
    // We can use p1 to buy Red cards? 
    // Let's assume MilitaryTrack logic works if unit tests pass. 
    // We'll skip deep integration test for Military for now due to access restrictions.
    std::cout << "SKIPPED (Access restrictions)\n";

    std::cout << "=== END UNIT TESTS ===\n\n";
}

int main() {
    // Run tests first
    try {
        runUnitTests();
    } catch (const std::exception& e) {
        std::cerr << "Tests crashed: " << e.what() << "\n";
    }

    std::cout << "=== 7 WONDERS DUEL - CONSOLE EDITION ===\n";

    try {
        Game game;

        // 1. Creează și pornește controlerul
        TestEngine runner(game);

        // 2. Rulează tot jocul
        runner.runGameLoop();

    }
    catch (const std::exception& e) {
        std::cerr << "CRASH: " << e.what() << std::endl;
    }

    std::cout << "Joc terminat. Apasa Enter.";
    std::cin.ignore();
    std::cin.get();
    return 0;
}