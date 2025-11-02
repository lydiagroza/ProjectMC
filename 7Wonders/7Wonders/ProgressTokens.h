#pragma once

#include<string>
#include<vector>
#include<map>

enum class ProgressTokenType {
	Agriculture,
	Architecture,
	Economy,
	Law,
	Mathematics,
	Masonry,
	Mathematics,
	Philosophy,
	Strategy,
	Theology,
	Urbanism
};

struct ProgressToken {
	std::string name;
	std::string description;
	int immediateVictoryPoints;
	int immediateCoins;
	int endGameVictoryPoints;

	ProgressToken(ProgressTokenType t, const std::string& n, const std::string& d, int vp = 0, int coins = 0, int multiplier = 0);
};


// takes tokens and current stats
// returneaza tuple cu updated (coins, VP).
// daca vrei sa aplici efectele token-urilor unui jucator:
// auto [newCoins, newVP] = calculateAllTokenEffects(player.getTokens(), player.getCoins(), player.getVP());
// player.setCoins(newCoins);
// player.setVP(newVP);
std::tuple<int, int> calculateAllTokenEffects(
	const std::vector<ProgressToken>& playerTokens, 
	int currentTokens, 
	int currentVitoryPoints, 
	const std::map<ProgressTokenType, 
	ProgressToken>& allTokens);