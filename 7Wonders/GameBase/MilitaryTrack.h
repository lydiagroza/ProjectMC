#pragma once
#include <cstdint>
#include "gameExport.h"

class  GAME_API MilitaryTrack
{
private:
    
	int8_t m_pawnPosition = 0; 
    static constexpr int8_t MAX_Position = 9;

	bool m_removedTokens[4] = { false, false, false, false };
	// player 1 token 5, player 1 token 2, player 2 token 2, player 2 token 5

    //maybe le pun in gameconstants ??
    static constexpr int TIER_1_POS = 2;
    static constexpr int TIER_1_REWARD = 2;

    static constexpr int TIER_2_START = 3;
    static constexpr int TIER_2_END = 5;
    static constexpr int TIER_2_REWARD = 5;

    static constexpr int TIER_3_START = 6;
    static constexpr int TIER_3_REWARD = 10;

public:
    MilitaryTrack() = default;

	int getPawnPosition() const;

    void applyShields(int platerId, int shields, int& coinsLostByOpponent);

    bool isTokenVisible(int tokenIndex) const;

	int getVictoryPointsForPlayer(int playerId) const;
};

