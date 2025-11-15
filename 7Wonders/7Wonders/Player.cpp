#include "Player.h"
#include "GameConstants.h"
Player::Player(const std::string& playerName) :m_name(playerName), m_nrCoins(GameConstants::STARTING_COINS), m_militaryScore(0), m_baseResources{}, m_pointsScore{}, m_discountedResource{}, m_chosenResource{}, cityBuildings{}, m_availableWonders{}, m_builtWonders{}, m_scientificSymbols{}, m_chainSymbols{} {
}

bool Player::decreaseCoins(std::uint8_t amount) {
	if (m_nrCoins >= amount) {
		m_nrCoins -= amount;
		return true;
	}
	return false;
}

void Player::addCoins(std::uint8_t amount) {
	m_nrCoins += amount;

}

void Player::add_Resource(Resource r, int amount)
{
	m_baseResources[r] += amount;
}
void Player::add_Points(Points p, int amount)
{
	m_pointsScore[p] += amount;
}
void Player::add_ScientificSymbol(Scientific_Symbol symbol)
{
	m_scientificSymbols.insert(symbol);
}
void Player::add_ChainSymbol(Symbol symbol)
{
	m_chainSymbols.insert(symbol);
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

				int costPerUnit = (GameConstants::TRADE_BASE_COST)+opponentProduction;
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



void Player::ProcessDiscardCard(const CardBase& c) {
	std::uint8_t gainedCoins = GameConstants::DISCARD_BASE_COINS;
std::string yellowKey = to_string(Color::Yellow);
	if (m_Inventory.count(yellowKey)) {
		std::uint8_t yellowCardsCount = m_Inventory.at(yellowKey).size();
		gainedCoins += yellowCardsCount * GameConstants::DISCARD_YELLOW_BONUS;
	}
	addCoins(gainedCoins);
}