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

public:
    MilitaryTrack() = default;

	int getPawnPosition() const;
};

