#include "Player.h"
#include<unordered_map>
Player::Player(const std::string& playerName) :m_name(playerName), m_nrCoins(7), m_militaryScore(0), m_baseResources{}, m_pointsScore{}, m_discountedResource {}, m_chosenResource{}, cityBuildings{}, m_availableWonders{}, m_builtWonders{}, m_scientificSymbols{}, m_chainSymbols{} {
}

bool Player::decreaseCoins(int amount) {
	if (m_nrCoins >= amount) {
		m_nrCoins -= amount;
		return true;
	}
	return false;
}


bool Player::hasSufficientResources(const CardBase& card) const {
	std::unordered_map<Resource, std::uint8_t> availableResources = m_baseResources;
	for (const auto& pair : m_chosenResource) {
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
	std::unordered_map<Resource, std::uint8_t> availableResources = m_baseResources;
	for (const auto& pair : m_chosenResource) {
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
			if (m_discountedResource.count(resource)) {
				totalTradeCost += missingAmount * m_discountedResource.at(resource);
			}
			else {
				int opponentProduction = 0;
				if (opponent.m_baseResources.count(resource)) {
					opponentProduction += opponent.m_baseResources.at(resource);
				}

				int costPerUnit = 2 + opponentProduction;
				totalTradeCost += missingAmount * costPerUnit;
			}

		}
	}
	return totalTradeCost;
}

bool Player::canAffordConstruction(const CardBase& c, const Player& opponent) {
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


void Player:: add_Resource(Resource r, int amount)
{
	m_baseResources[r] += amount;
}
void Player:: add_Coin(int amount)
{
	m_nrCoins += amount;
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



