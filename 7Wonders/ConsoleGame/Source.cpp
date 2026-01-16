#include <iostream>
#include <memory>
#include <chrono>
#include <filesystem> // For absolute path
#include "Game.h"
#include "AI_Player.h"
#include "CardBase.h"
#include "Wonder.h"

namespace fs = std::filesystem;

// --- AI TRAINING LOOP ---
void trainAI(int episodes) {
    std::cout << "=== STARTING AI TRAINING (" << episodes << " episodes) ===\n";
    auto startTime = std::chrono::high_resolution_clock::now();

    int p1Wins = 0;
    int p2Wins = 0;
    
    // Stats for the last played game
    int lastVP1 = 0;
    int lastVP2 = 0;
    std::string lastWinner = "Draw";

    for (int i = 0; i < episodes; ++i) {
        Game game;
        
        // 1. Setup AI vs AI (Adaptive/Hard for training)
        game.setPlayerTypes(true, true, AI_Difficulty::ADAPTIVE); 
        game.initialize();

        auto p1 = std::dynamic_pointer_cast<AI_Player>(std::shared_ptr<Player>(std::shared_ptr<Player>(), &game.getPlayer1()));
        auto p2 = std::dynamic_pointer_cast<AI_Player>(std::shared_ptr<Player>(std::shared_ptr<Player>(), &game.getPlayer2()));

        if (p1) p1->enableTraining(true);
        if (p2) p2->enableTraining(true);

        // 2. Draft Phase Simulation
        while (game.getDraftPhase() > 0 && !game.getCurrentDraftSet().empty()) {
            AI_Player* currentAI = dynamic_cast<AI_Player*>(game.getCurrentPlayer());
            if (currentAI) {
                auto chosenWonder = currentAI->chooseWonderFromDraft(game.getCurrentDraftSet());
                if (chosenWonder) {
                    game.draftWonder(chosenWonder->getId());
                } else {
                    game.draftWonder(game.getCurrentDraftSet()[0]->getId());
                }
            }
        }

        // 3. Gameplay Loop
        while (!game.isGameOver()) {
            AI_Player* currentAI = dynamic_cast<AI_Player*>(game.getCurrentPlayer());
            Player* opponent = game.getOpponent();

            if (currentAI && opponent) {
                currentAI->makeDecision(game.getBoard(), *opponent, game.getCurrentAge());

                if (currentAI->hasExtraTurn()) {
                    currentAI->setHasExtraTurn(false); 
                } else {
                    game.switchTurn();
                }
            }

            if (game.isEndOfAge()) {
                game.startNextAge();
            }
        }

        // 4. End Game & Reward
        lastVP1 = game.calculatePlayerVP(game.getPlayer1());
        lastVP2 = game.calculatePlayerVP(game.getPlayer2());
        bool p1Won = (lastVP1 > lastVP2); 
        
        if (lastVP1 > lastVP2) lastWinner = "Player 1 (AI)";
        else if (lastVP2 > lastVP1) lastWinner = "Player 2 (AI)";
        else lastWinner = "Draw";

        if (p1) p1->onGameEnd(p1Won, lastVP1);
        if (p2) p2->onGameEnd(!p1Won, lastVP2);

        if (p1Won) p1Wins++; else if (lastVP2 > lastVP1) p2Wins++;

        if ((i + 1) % 100 == 0) {
            std::cout << "Episode " << i + 1 << "/" << episodes 
                      << " | P1 Wins: " << p1Wins << " | P2 Wins: " << p2Wins << "\r" << std::flush;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;

    std::cout << "\n\n=== TRAINING COMPLETE ===\n";
    std::cout << "Time Elapsed: " << elapsed.count() << "s\n";
    std::cout << "Total Games: " << episodes << "\n";
    std::cout << "Player 1 Wins: " << p1Wins << " (" << (p1Wins * 100.0 / episodes) << "%)\n";
    std::cout << "Player 2 Wins: " << p2Wins << " (" << (p2Wins * 100.0 / episodes) << "%)\n";
    
    std::cout << "\n=== LAST MATCH RESULT ===\n";
    std::cout << "Winner: " << lastWinner << "\n";
    std::cout << "Player 1 VP: " << lastVP1 << "\n";
    std::cout << "Player 2 VP: " << lastVP2 << "\n";

    fs::path modelPath = fs::absolute("ai_model.txt");
    std::cout << "\n[System] AI Model saved to: " << modelPath.string() << "\n";
}

int main() {
    trainAI(1000);

    std::cout << "\nPress Enter to exit...";
    std::cin.ignore();
    std::cin.get();
    return 0;
}