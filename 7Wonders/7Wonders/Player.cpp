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
 
//Functii pentru monede
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


//Functii pentru resurse
void Player::add_Resource(Resource r, std::uint8_t amount)
{
	m_Resources[r] += amount;
}

void Player::add_DiscountedResource(Resource r, std::uint8_t discount) {
	m_discountedResource[r] = discount;
}

//Functii pentru puncte si simboluri
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




//Functie care imi da resursele de baza(doar alea din cartile maro si gri),exludem coins ca sa nu incurce
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
	std::uint8_t opponentResourceProduction = opponentBaseProduction.count(r) ? opponentBaseProduction.at(r) : 0;

	return GameConstants::TRADE_BASE_COST + opponentResourceProduction;
}



//Functia pentru a obtine ce resurse putem alege
std::vector<std::vector<Resource>> Player::getFlexibleChoices() const {

	std::vector<std::vector<Resource>> choices;

	/*if (m_Inventory.count(Color::Yellow)) {
		for (const auto& card : m_Inventory.at(Color::Yellow)) {
			for (const auto& option : card.getFlexibleChoices()) {  //getFlexibleChoise ar trebuie sa returneze un vector cu resursele disponile(Doar pentru cartile galbene)
				if (!option.empty()) {
					choices.push_back(option);
				}
			}
		}
	}


	
	for (const auto& wonder : m_Wonders) {
		
		for (const auto& option : wonder.getFlexibleChoices()) {  //getFlexibleChoise ar trebuie sa returneze un vector cu resursele disponile(Doar pentru Wonders cu acest efect de alegere a resursei)
			if (!option.empty()) {
				choices.push_back(option);
			}
		}
	}*/

	return choices;
}

std::string Player::getName() const {
	return m_name;
}


//Functie care ne zice resursele lipsa pentru cartea respectiva
std::map<Resource, std::uint8_t> Player::getMissingResources(const CardBase& card, const Player& opponent) const {
	std::map<Resource, std::uint8_t> required = card.get_cost();
	std::map<Resource, std::uint8_t> missing;
	std::map<Resource, std::uint8_t> currentBaseProduction = this->getBaseProduction();

	for (const auto& [res, reqAmount] : required) {
		if (res == Resource::Coin) continue;

		uint8_t available = currentBaseProduction.count(res) ? currentBaseProduction.at(res) : 0;

		if (available < reqAmount) {
			missing[res] = reqAmount - available;
		}
	}

	std::vector<std::vector<Resource>> flexibleChoices = this->getFlexibleChoices();
	for (const auto& choiceOptions : flexibleChoices) {

		Resource bestResourceToCover = Resource::Coin; 
		std::uint8_t maxCost = -1;

		// Găsim cea mai SCUMPĂ resursă lipsă pe care o putem acoperi
		for (Resource resToCover : choiceOptions) {

			if (missing.count(resToCover) && missing.at(resToCover) > 0) {

				int currentCost = this->getUnitTradeCost(resToCover, opponent);

				// Alegem opțiunea care ne salvează cei mai mulți bani (Max Cost)
				if (currentCost > maxCost) {
					maxCost = currentCost;
					bestResourceToCover = resToCover;
				}
			}
		}

		// Dacă s-a găsit o alocare optimă (resursa nu este Coin)
		if (bestResourceToCover != Resource::Coin) {

			// Consumăm o unitate din lipsa cea mai scumpă
			missing.at(bestResourceToCover)--;

			if (missing.at(bestResourceToCover) == 0) {
				missing.erase(bestResourceToCover);
			}
		}
	}

	return missing;

}


//Functia care ne zice cati banuti trebuie sa dea pentru resursele lipsa
std::uint8_t Player::calculateTradeCost(const CardBase& card, const Player& opponent)const {
	const auto missingResources = this->getMissingResources(card, opponent);
	std::uint8_t totalTradeCost = 0;
	for (const auto& [res, missingAmount] : missingResources) {
		int costPerUnit = this->getUnitTradeCost(res, opponent);
		totalTradeCost += missingAmount * costPerUnit;
	}
	return totalTradeCost;
}


//Functie care ne zice efectiv daca isi permite cartea sau nu
bool Player::canAffordConstruction(const CardBase& card, const Player& opponent) {
	std::uint8_t cardCoinCost = card.getCostForResource(Resource::Coin);
	std::uint8_t tradeCost = this->calculateTradeCost(card, opponent);
	std::uint8_t totalCost = cardCoinCost + tradeCost;
	if (m_Resources.count(Resource::Coin) && m_Resources.at(Resource::Coin) >= totalCost) {
		return true;
	}
	return false;
}




//Functie care cumpara cartea 
bool Player::buyCard(const CardBase& card, const Player& opponent, const Board &board) { 
	bool isFreebyChain= card.m_unlocks.has_value() && m_chainSymbols.count(card.m_unlocks.value());
	std::uint8_t totalCoinCost = 0;
	if (isFreebyChain == false) {
		if (!this->canAffordConstruction(card, opponent)) {
			std::cout << "Card " << card.get_name() << " can't be bought: Insufficient funds or resource trading cost is too high." << std::endl;
			return false;
		}
	}
	totalCoinCost = card.getCostForResource(Resource::Coin) + this->calculateTradeCost(card, opponent);
	if (totalCoinCost > 0) {

		m_Resources.at(Resource::Coin) -= totalCoinCost;
	}
	m_Inventory[card.m_color].push_back(card);
	card.applyEffect(*this, opponent, board);
	std::cout << "Card " << card.get_name() << " constructed successfully. Cost paid: " << totalCoinCost << " coins." << std::endl;
	return true;
	
	}

	

//Functie in cazul in care jucatorul alege sa arda cartea pentru banuti
void Player::discardCard(const CardBase& card) {

	std::uint8_t gainedCoins = GameConstants::DISCARD_BASE_COINS;
	gainedCoins += m_Inventory[Color::Yellow].size() * GameConstants::DISCARD_YELLOW_BONUS;
	addCoins(gainedCoins);
}




//void Player::constructWonder(const CardBase& cardUsed, const Wonder& wonderToBuild, Player& opponent, Board& board) {
	/*board.incrementWondersBuilt();*/ // ---> avem un bool la wonders. daca e sau nu built ca sa nu avem 2 mapuri de wonders 
//if (board.getTotalWondersBuilt() == GameConstants::WONDER_LIMIT) {

	// Logica: Cea de-a 8-a Minune r?mas? neconstruit? este scoas? din joc.
	//Aceste  functii legate de Wonder trebuie implementate ori in Board ori in starea curenta a jocului
	//board.discardLastAvailableWonder();
//}
//m_builtWonders.push_back(wonderToBuild);
//wonderToBuild.applyEffect(*this, opponent, board);

//}


