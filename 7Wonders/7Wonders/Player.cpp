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

//Functie de get pentru numele playerului
std::string Player::getName()const {
	return m_name;
}

//Functii pentru  gestionarea monedelor
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

void Player::add_DiscountedResource(Resource r) { 
	m_discountedResource.insert (r) ;
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





//Functie pentru a afla costul unei singure resurse de care jucatorul are nevoie  
std::uint8_t Player::getUnitTradeCost(Resource r, const Player& opponent) const {
	
	if (m_discountedResource.count(r)) {
		return 1;
	}

	std::uint8_t opponentResourceProduction = opponent.m_Resources.count(r) ? opponent.m_Resources.at(r) : 0;

	return GameConstants::TRADE_BASE_COST + opponentResourceProduction;
}




//Functie care returneaza un map cu resursele lipsa pentru o carte 
std::map<Resource, std::uint8_t> Player::getMissingResources(const CardBase& card, const Player& opponent) const {
	
	std::map<Resource, std::uint8_t> missingResources;
	
	for (const auto& [res, reqAmount] : card.get_cost()) {
		if (res == Resource::Coin) continue;
		
		uint8_t availableResource = m_Resources.count(res) ? m_Resources.at(res) : 0;

		if (availableResource < reqAmount) {
			missingResources[res] = reqAmount - availableResource;
		}
	}

	for (const auto& [sourceName,choiceOptions] : m_flexibleResources) {

		Resource bestResourceToCover = Resource::Coin; 
		std::uint8_t maxCost = -1;

		// Găsim cea mai SCUMPĂ resursă lipsă pe care o putem acoperi
		for (Resource resToCover : choiceOptions) {

			if (missingResources.count(resToCover) && missingResources.at(resToCover) > 0) {

				std::uint8_t currentCost = this->getUnitTradeCost(resToCover, opponent);

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
			missingResources.at(bestResourceToCover)--;

			if (missingResources.at(bestResourceToCover) == 0) {
				missingResources.erase(bestResourceToCover);
			}
		}
	}

	return missingResources;

}


//Functia care ne zice cati banuti trebuie sa dea pentru resursele lipsa (excludem banutii pentru ca ei nu fac parte din logica de comert)
std::uint8_t Player::calculateTradeCost(const CardBase& card, const Player& opponent)const {
	const auto missingResources = this->getMissingResources(card, opponent);
	std::uint8_t totalTradeCost = 0;
	for (const auto& [res, missingAmount] : missingResources) {
		if (res == Resource::Coin) {
			continue;
		}
		std::uint8_t costPerUnit = this->getUnitTradeCost(res, opponent);
		totalTradeCost += missingAmount * costPerUnit;
	}
	return totalTradeCost;
}



//Functie care returneaza costul pentru  a cumpara cartea 
std::uint8_t Player::getTotalCardCost (const CardBase& card, const Player& opponent) {
	if (card.m_unlocks.has_value() && m_chainSymbols.count(card.m_unlocks.value())) {
		return 0; // Costul este 0 (Gratuit)
	}
	std::uint8_t totalCost = this->calculateTradeCost(card, opponent) + card.getCostForResource(Resource::Coin);
	if (m_Resources.count(Resource::Coin) && m_Resources.at(Resource::Coin) >= totalCost) {
		return totalCost;
	}
	return -1;
	
}




//Functie care cumpara cartea 
bool Player::buyCard(std::shared_ptr<CardBase> card, const Player& opponent, const Board &board) {
	std::uint8_t totalCoinCost = this->getTotalCardCost(*card, opponent);
	if (totalCoinCost == 0) {
		std::cout << "Card " << card->get_name() << "is free." << std::endl;
	}
	if (totalCoinCost ==-1) {
		std::cout << "Card " << card->get_name() << " can't be bought: Insufficient funds or resource trading cost is too high." << std::endl;
		return false;
	}
	if (totalCoinCost > 0) {
		m_Resources[Resource::Coin] -= totalCoinCost;
	}

	m_Inventory[card->m_color].push_back(card);
	card->applyEffect(*this, opponent, board);
	std::cout << "Card " << card->get_name() << " constructed successfully. Cost paid: " << totalCoinCost << " coins." << std::endl;
	return true;

}

// gestionare inventar
void Player::addCardToInventory(std::shared_ptr<CardBase> card) {
	m_Inventory[card->m_color].push_back(card);
}

bool Player::removeCardFromInventory(std::shared_ptr<CardBase> card) {
    auto& cards = m_Inventory[card->m_color];
	for (auto it = cards.begin(); it != cards.end(); ++it) {
		if ((*it)->get_id() == card->get_id()) {
			cards.erase(it);
			return true;
		}
	}
    return false;
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


