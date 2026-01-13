#pragma once
#include "Game.h"

class TestEngine {
public:
    TestEngine(Game& game);

    void runWonderDraftLoop();

    void runGameLoop();
private:
    Game& m_game; 

    
    void runAllUnitTests();

};