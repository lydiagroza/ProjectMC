#include "Player.h"
#include "GameConstants.h"

//Constructor
Player::Player(const std::string& playerName)
	:m_name(playerName),
	m_Resources{},
	m_pointsScore{},
	m_discountedResource{},
	m_Wonders{},
	m_scientificSymbols{},
	m_chainSymbols{} {
}
 
//FUNCTII PENTRU MONEDE
bool Player::decreaseCoins(std::uint8_t amount) { 
	if (m_Resources[Coin] >= amount) {
		m_Resources[Coin] -= amount;
		return true ; 
	}
	std::cout << "Insufficient coins";
	return false; 
}

void Player::addCoins(std::uint8_t amount) {
	m_Resources[Coin] += amount;

}


//FUNCTII PENTRU RESURSE
void Player::add_Resource(Resource r, std::uint8_t amount)
{
	m_Resources[r] += amount;
}

void Player::add_DiscountedResource(Resource r, std::uint8_t discount) {
	m_discountedResource[r] = discount;
}

//FUNCTII PENTRU PUNCTE SI SIMBOLURI
void Player::add_Points(Points p, std::uint8_t amount)
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

//Functie care imi da resursele de baza,exludem coins ca sa nu incurce
std::map<Resource, std::uint8_t> Player::getBaseProduction() const {
	std::map<Resource, std::uint8_t> fixedProduction;
	for (const auto& [res, amount] : m_Resources) {
		if (res != Resource::Coin) {
			fixedProduction[res] = amount;
		}
	}
	
	return fixedProduction;
}

//Functie pentru a afla costul unei singure resurse
std::uint8_t Player::getUnitTradeCost(Resource r, const Player& opponent) const {
	
	if (m_discountedResource.count(r)) {
		return 1;
	}

	const auto opponentBaseProduction = opponent.getBaseProduction();
	int opponentResourceProduction = opponentBaseProduction.count(r) ? opponentBaseProduction.at(r) : 0;

	return GameConstants::TRADE_BASE_COST + opponentResourceProduction;
}


bool hasSufficientResources(const std::map<Resource, std::uint8_t>& a,const std::map<Resource, uint8_t>& b){

	for (const auto& [res, valueA] : a) {
		if (valueA < b.at(res)) return false;
	}
	return true;
}

bool Player::buyCard(const CardBase& card, const Player& opponent, const Board &board) {  
	if (hasSufficientResources(m_Resources, card.get_cost())) {
		m_Resources[Coin] -= card.getCostForResource(Coin);
		m_Inventory[card.get_color()].push_back(card);
		card.applyEffect(*this, opponent, board);
		std::cout << "Card " << card.get_name() << " bought" << std::endl; // + se "construieste" cartea 
		return true; 
	}
	else {  // implementare cu cartile galbene , verifici daca ai adauga orice resursa din cartile galbene daca se modifica cv

		// daca nu intra pe urmatoarea ramura pt ca aici vom avea un return: 
		// aici bagi si trade cost 
		return;
	}
	
			std::cout << "Card can't be bought"; 
			return false;

	}

	/*for (const auto& pair : m_chosenResource) {
		availableResources[pair.first] += pair.second;
	}*/
	
	/*const auto& requiredResources = card.get_cost();*/
	/*for (const auto& pair : requiredResources) {
		const Resource resource = pair.first;
		int requiredAmount = pair.second;
		if (resource == Resource::Coin) {
			continue;
		}*/

	//	int possessedAmount = availableResources.count(resource) ? availableResources.at(resource) : 0;
	//	if (possessedAmount < requiredAmount) {
	//		return false;
	//	}
	//}
	//return true;


int Player::calculateTradeCost(const CardBase& card, const Player& opponent) const {

	const auto& requiredResources = card.get_cost(); // nu mai trb inca o variabila sa retii costul aici 
	int totalTradeCost = 0;
	std::map<Resource, std::uint8_t> availableResources = m_Resources; // avem m_Resources pt asta, nu trb alta variabila
	//astea vor fi parcurse in alta fct mai sus inainte sa se apeleze fct asta 
	/*for (const auto& pair : m_chosenResource) {
		availableResources[pair.first] += pair.second;
	}*
	/*for (const auto& pair : requiredResources) {
		const Resource resource = pair.first;
		int requiredAmount = pair.second;
		if (resource == Resource::Coin) {
			continue;
		}*/

		// ---------------> alta fct care face de cate resurse mai avem nevoie 
		// 	int missingAmount = requiredAmount - availableResources[resource];

		/*	if (missingAmount > 0) {
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

		return totalTradeCost;
	}*/
}

// asta nu mai trb e fcta in teorie in buy card 
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


void Player::discardCard(const CardBase& c) {

	std::uint8_t gainedCoins = GameConstants::DISCARD_BASE_COINS;
	gainedCoins += m_Inventory[Color::Yellow].size() * GameConstants::DISCARD_YELLOW_BONUS;
	addCoins(gainedCoins);
}







//aici nu ma bag ca nush wonders :((((
void Player::constructWonder(const CardBase& cardUsed, const Wonder& wonderToBuild, Player& opponent, Board& board) {
	/*board.incrementWondersBuilt();*/ // ---> avem un bool la wonders. daca e sau nu built ca sa nu avem 2 mapuri de wonders 
	if (board.getTotalWondersBuilt() == GameConstants::WONDER_LIMIT) {
		
		// Logica: Cea de-a 8-a Minune r?mas? neconstruit? este scoas? din joc.
		//Aceste  functii legate de Wonder trebuie implementate ori in Board ori in starea curenta a jocului
		board.discardLastAvailableWonder();
	}
	m_builtWonders.push_back(wonderToBuild);
	wonderToBuild.applyEffect(*this, opponent, board);
}

