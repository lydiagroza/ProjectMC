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
int Player::calculateTradeCost( const CardBase & c,  const Player& opponent)const {
	int totalTradeCost = 0;
	
	for (const auto& pair : c.get_cost) {
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
	return totalTradeCost;dxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxcwssssssssssssssssssssssssssssssssssssss             vvvvvvvvvvvvv                                                                                                                                                                                                                                               
}
bool Player::canAffordConstruction(const CardBase &c , const Player& opponent) {
	if (check_resources(c)) return true; 
	else {
		int tradeCost = calculateTradeCost(c, opponent);
		int totalCost = tradeCost;
		auto it = c.get_cost().find(Resource::Coin);
		if (it != c.get_cost().end()) {
			 totalCost += it->second;
		}
	
		if (decreaseCoins(totalCost)) {
			return true;
		}
		return false;
	}
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

void Player:: add_Resource(Resource r, int amount)
{
	m_inventory[r] += amount;
}
void Player:: add_Points(Points p, int amount)
{
	m_pointsScore[p] += amount;
}
void Player:: add_ScientificSymbol(Scientific_Symbol symbol)
{
	m_scientificSymbols.insert(symbol);
}
void Player:: add_ChainSymbol(Symbol symbol)
{
	m_chainSymbols.insert(symbol);
}



