#pragma once
#include "Board.h"

class GameSetup
{
private:
	Board board;

/// <summary>
/// /tokeni hardcodati temporar pana sunt gata ceilalti
/// </summary>
    std::vector<ProgressToken> allTokens = {
       {"Agriculture", "Gain 6 coins and 4 victory points.", 4, 6, 0},
       {"Architecture", "Wonders cost 2 fewer stone/brick/wood.", 0, 0, 0},
       {"Economy", "Gain coins whenever your opponent gains coins.", 0, 0, 0},
       {"Law", "Counts as a science symbol.", 0, 0, 0},
       {"Mathematics", "Gain 3 victory points per progress token at end of game.", 0, 0, 3},
       {"Masonry", "Wonders cost 2 fewer stone/brick.", 0, 0, 0},
       {"Philosophy", "Gain 7 victory points.", 7, 0, 0},
       {"Strategy", "Your shields gain +1 strength.", 0, 0, 0},
       {"Theology", "Your wonders activate their effects immediately.", 0, 0, 0},
       {"Urbanism", "Gain 6 coins immediately.", 0, 6, 0}
    };
};


public:
	GameSetup();
    std::vector<ProgressToken> selectProgressTokens();
    
    Board& getBoard();

};

