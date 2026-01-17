#include "MilitaryTrack.h"
#include <algorithm>

int MilitaryTrack::getPawnPosition() const
{
    return m_pawnPosition;
}

void MilitaryTrack::applyShields(int playerId, int shields, int& coinsLostByOpponent)
{
	int8_t oldPosition = m_pawnPosition;
    int direction = (playerId == 1) ? 1 : -1;
	int temporaryPos = m_pawnPosition+ direction * shields;
	m_pawnPosition = std::clamp((int8_t)temporaryPos, (int8_t)-MAX_Position, MAX_Position);
	//ma face sa nu pot depasi pozitia minima sau maixima -> o face sa fie in limitele impuse

	if(std::abs(m_pawnPosition) == MAX_Position)
	{
		return;
	}

	bool isAdvancing = false;
	if (direction == 1 && m_pawnPosition > oldPosition)
		isAdvancing = true;
	else if (direction == -1 && m_pawnPosition < oldPosition)
		isAdvancing = true;

	if (isAdvancing)
	{
		
        int absOld = std::abs(oldPosition);
        int absNew = std::abs(m_pawnPosition);

        if (absOld < 3 && absNew >= 3)
        {
            
            int tokenIdx = (playerId == 1) ? 2 : 1;

            if (!m_removedTokens[tokenIdx]) {
                coinsLostByOpponent += 2;
                m_removedTokens[tokenIdx] = true;
            }
        }

        if (absOld < 6 && absNew >= 6)
        {
            
            int tokenIdx = (playerId == 1) ? 3 : 0;

            if (!m_removedTokens[tokenIdx]) {
                coinsLostByOpponent += 5;
                m_removedTokens[tokenIdx] = true;
            }
        }
	}
	return;

}

bool MilitaryTrack::isTokenVisible(int tokenIndex) const
{
   if(tokenIndex < 0 || tokenIndex >= 4)
	   return false;
   return !m_removedTokens[tokenIndex];
}

int MilitaryTrack::getVictoryPointsForPlayer(int playerId) const
{
	int pos = static_cast<int>(m_pawnPosition);
	int distance = std::abs(pos);
	bool isScoring = (playerId == 1 && pos > 0) || (playerId == 2 && pos < 0);
	if(isScoring)
	{
        if (distance >= TIER_3_START) {
            return TIER_3_REWARD;
        }
        
        else if (distance >= TIER_2_START && distance <= TIER_2_END) {
            return TIER_2_REWARD;
        }
       
        else if (distance == TIER_1_POS) {
            return TIER_1_REWARD;
        }
	}
	return 0;
}
