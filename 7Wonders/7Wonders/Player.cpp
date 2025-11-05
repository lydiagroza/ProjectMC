#include "Player.h"
#include<unordered_map>
Player::Player(const std::string& playerName) :name(playerName), nrCoins(7), militaryScore(0), rawMaterials{}, manufacturedGoods{}, fixedCost{}, bonusProduction{}, cityBuildings{}, availableWonders{}, builtWonders{}, scientificSymbols{}, chainSymbols{} {
}

bool Player::decreaseCoins(int amount) {
	if (nrCoins >= amount) {
		nrCoins -= amount;
		return true;
	}
	return false;
}


bool Player::hasSufficientResources(const CardBase& card) const {
	std::unordered_map<Resource, int> availableResources = baseProduction;
	for (const auto& pair : bonusProduction) {
		availableResources[pair.first] += pair.second;
	}

	const auto& requiredResources = card.get_cost();
	for (const auto& pair : requiredResources) {
		const Resource resource = pair.first;
		int requiredAmount = pair.second;
		if (resource == Resource::Coin) {
			continue;
		}

		int possessedAmount = availableResources.count(resource) ? availableResources.at(resource) : 0;
		if (possessedAmount < requiredAmount) {
			return false;
		}
	}
	return true;
}


int Player::calculateTradeCost(const CardBase& card, const Player& opponent) const {

	if (hasSufficientResources(card) == true) {
		return 0;
	}
	const auto& requiredResources = card.get_cost();
	int totalTradeCost = 0;
	std::unordered_map<Resource, int> availableResources = baseProduction;
	for (const auto& pair : bonusProduction) {
		availableResources[pair.first] += pair.second;
	}

	for (const auto& pair : requiredResources) {
		const Resource resource = pair.first;
		int requiredAmount = pair.second;
		if (resource == Resource::Coin) {
			continue;
		}
		int missingAmount = requiredAmount - availableResources[resource];

		if (missingAmount > 0) {
			if (fixedCost.count(resource)) {
				totalTradeCost += missingAmount * fixedCost.at(resource);
			}
			else {
				int opponentProduction = 0;
				if (opponent.baseProduction.count(resource)) {
					opponentProduction += opponent.baseProduction.at(resource);
				}

				int costPerUnit = 2 + opponentProduction; 
				totalTradeCost += missingAmount * costPerUnit;
			}

		}
	}
	return totalTradeCost;
}

bool Player::canAffordConstruction(const Card& c,const Player& opponent) {
	int CardCoinCost = 0;
	const auto& cardCost = c.get_cost();
	if (cardCost.count(Resource::Coin)) {
		CardCoinCost = cardCost.at(Resource::Coin);
	}
	int tradeCost = calculateTradeCost(c, opponent);
	int totalCost = CardCoinCost + tradeCost;
	if (decreaseCoins(totalCost)) {
		return true;
	}
	return false;
	
 }
//other functions





