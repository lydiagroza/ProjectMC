#include "ProgressTokens.h"

#include <iostream>
#include<algorithm>

ProgressToken::ProgressToken(ProgressTokenType t, const	std::string& n, const std::string& d, int vp, int coins, int multiplier)
	: type(t), name(n), description(d), immediateVictoryPoints(vp), immediateCoins(coins), endGameVictoryPoints(multiplier) {}

// hardcodare tokens (soon)
//			|	 |	  |
//         \|/  \|/  \|/    
//			V    V    V