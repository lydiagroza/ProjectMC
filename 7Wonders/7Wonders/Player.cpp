#include"Player.h"
#include<unordered_map>
Player::Player(const std::string& playerName) :name(playerName), nrCoins(7), militaryScore(0), rawMaterials{}, manufacturedGoods{}, fixedCost{}, bonusProduction{}, cityBuildings{}, availableWonders{}, builtWonders{}, scientificSymbols{}, chainSymbols{} {
}
void Player::addCoins(int amount) {
	nrCoins += amount;
}
bool Player::decreaseCoins(int amount) {
	if (nrCoins >= amount) {
		nrCoins -= amount;
		return true;
	}
	return false;
}
int Player::calculateTradeCost(const std::unordered_map < std::string, int>& requiredResurces, const Player& opponent)const {
	int totalTradeCost = 0;
	std::unordered_map<std::string, int>availableResurces = rawMaterials;
	for(const auto&pair:manufacturedGoods) {
		availableResurces[pair.first] += pair.second;
	}
	for(const auto&pair:bonusProduction) {
		availableResurces[pair.first] += pair.second;
	}
	for (const auto& pair : requiredResurces) {
		const std::string& resource = pair.first;
		int requiredAmount = pair.second;
		int missingAmount = requiredAmount - availableResurces[resource];
		if (missingAmount > 0) {
			if (fixedCost.count(resource)) {
				totalTradeCost += missingAmount * fixedCost.at(resource);
			}
			else {
				int opponentProduction = 0;
				if (opponent.rawMaterials.count(resource)) {
					opponentProduction += opponent.rawMaterials.at(resource);
				}
				if (opponent.manufacturedGoods.count(resource)) {
					opponentProduction += opponent.manufacturedGoods.at(resource);
				}
				int costPerUnit = 2 + opponentProduction;
				totalTradeCost += missingAmount * costPerUnit;
			}
			
		}
	}
	return totalTradeCost;
}
bool Player::canAffordConstruction(int fixedCoinCost, const std::unordered_map<std::string, int>& requiredResources,const Player& opponent) {
	int tradeCost = calculateTradeCost(requiredResources, opponent);
	int totalCost = fixedCoinCost + tradeCost;
	if(decreaseCoins(totalCost)) {
		return true;
	}
	return false;
 }

//void Player::moveMilitaryScore(int shields,Player&opponent) {
//	militaryScore += shields;
//	if (militaryScore > 9) {
//		militaryScore = 9;
//	}
//	if (militaryScore == 9) {
//		return;
//	}
//}




