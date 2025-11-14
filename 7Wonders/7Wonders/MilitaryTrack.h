#pragma once
#include <cstdint>


struct MilitaryResult
{
    bool supremacyAchieved = false;
    int coinsLostByOpponent = 0; 
    bool tokenRemoved = false;
};

class MilitaryTrack
{
private:
    
	int8_t m_pawnPosition = 0; 
    static constexpr int8_t MAX_Position = 9;

	bool m_removedTokens[4] = { false, false, false, false };
	// player 1 token 5, player 1 token 2, player 2 token 2, player 2 token 5

public:
    MilitaryTrack() = default;

	int getPawnPosition() const;

	MilitaryResult applyShields(int platerId, int shields);
};

