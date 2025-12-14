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

//Resursele de baza
const std::map<Resource, std::uint8_t>& Player::getResources() const {
	return m_Resources;
}

//Punctele
const std::map<Points, std::uint8_t>& Player::getPoints() const {
	return m_pointsScore;
}

//Inventarul de carti
const std::map<Color, std::vector<std::shared_ptr<CardBase>>>& Player::getInventory() const {
	return m_Inventory;  
}

//Wonders
const std::vector<Wonder>& Player::getWonders() const {
	return m_Wonders;  
}

//ScientificSymbols
const std::unordered_set<Scientific_Symbol>& Player::getScientificSymbols() const {
	return m_scientificSymbols;
}

//Extra turn
bool Player::hasExtraTurn() const {
	return m_hasExtraTurn;  
}

//ChainSymbols
const std::unordered_set<Symbol>& Player::getChainSymbols() const {
	return m_chainSymbols;
}

//Coins
 std::uint8_t Player::getCoins()  {
	return m_Resources[Coin];
}

//Functii pentru  gestionarea monedelor
bool Player::decreaseCoins(std::uint8_t amount) {
	if (m_Resources[Coin] >= amount) {
		m_Resources[Coin] -= amount;
		return true;
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

void Player::set_discountedResource(Resource r) {
	int index = findResourceDiscountIndex(r);
	if (index != -1) {
		m_discountedResource[index] = 1;
	}
}

void Player::removeResource(Resource r, std::uint8_t amount) {
	if (m_Resources[r] >= amount) {
		m_Resources[r] -= amount;
	}
	else {
		m_Resources[r] = 0;
		std::cout << "Warning: Tried to remove more resources than available" << std::endl;
	}
}

void Player::add_FlexibleResource(const std::vector<Resource>& options) {
	std::bitset<5> flexibleSet;

	// Setăm biții corespunzători resurselor
	for (Resource res : options) {
		int index = findResourceDiscountIndex(res);
		if (index != -1) {
			flexibleSet[index] = 1;
		}
	}

	m_flexibleResources.push_back(flexibleSet);
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


int Player::findResourceDiscountIndex(Resource r) const {
	switch (r) {
	case Resource::Wood: return 0;
	case Resource::Clay: return 1;
	case Resource::Stone: return 2;
	case Resource::Glass: return 3;
	case Resource::Papyrus: return 4;
	default: return -1;
	}
}

Resource Player::findResourceDiscountFromIndex(int index) const {
	switch (index) {
	case 0: return Resource::Wood;
	case 1: return Resource::Clay;
	case 2: return Resource::Stone;
	case 3: return Resource::Glass;
	case 4: return Resource::Papyrus;
	default: return Resource::Coin; // Fallback
	}
}


//Functie pentru a afla costul unei singure resurse de care jucatorul are nevoie  
std::uint8_t Player::findUnitTradeCost(Resource r, const Player& opponent) const {

	int index = findResourceDiscountIndex(r);
	if (index != -1 && m_discountedResource[index] == 1) {
		return 1;
	}

	std::uint8_t opponentResourceProduction = opponent.m_Resources.count(r) ? opponent.m_Resources.at(r) : 0;

	return GameConstants::TRADE_BASE_COST + opponentResourceProduction;
}




//Functie care returneaza un map cu resursele lipsa pentru o carte 
std::map<Resource, std::uint8_t> Player::MissingResources(
	const std::map<Resource, std::uint8_t>& requiredResources,
	const Player& opponent) const {

	std::map<Resource, std::uint8_t> missingResources;

	// Calculăm resursele lipsă (NESCHIMBAT)
	for (const auto& [res, reqAmount] : requiredResources) {
		if (res == Resource::Coin) continue;

		uint8_t availableResource = m_Resources.count(res) ? m_Resources.at(res) : 0;

		if (availableResource < reqAmount) {
			missingResources[res] = reqAmount - availableResource;
		}
	}

	// ✅ NOU: Folosim bitset în loc de map<string, vector>
	for (const auto& flexibleSet : m_flexibleResources) {
		//                ^^^^^^^^^^^
		//                Fiecare bitset = un set de opțiuni

		Resource bestResourceToCover = Resource::Coin;
		std::uint8_t maxCost = 0;

		// Iterăm prin cei 5 biți (0=Wood, 1=Clay, 2=Stone, 3=Glass, 4=Papyrus)
		for (int i = 0; i < 5; ++i) {
			if (flexibleSet[i] == 0) continue;  // Acest bit = 0 → resursa nu e disponibilă

			// Convertim index → Resource
			Resource resToCover = findResourceDiscountFromIndex(i);

			// Verificăm dacă avem nevoie de această resursă
			if (missingResources.count(resToCover) && missingResources.at(resToCover) > 0) {
				std::uint8_t currentCost = this->findUnitTradeCost(resToCover, opponent);

				// Alegem cea mai scumpă resursă
				if (currentCost > maxCost) {
					maxCost = currentCost;
					bestResourceToCover = resToCover;
				}
			}
		}

		// Dacă s-a găsit o alocare optimă
		if (bestResourceToCover != Resource::Coin) {
			missingResources.at(bestResourceToCover)--;

			if (missingResources.at(bestResourceToCover) == 0) {
				missingResources.erase(bestResourceToCover);
			}
		}
	}

	return missingResources;
}


//Functia care ne zice cati banuti trebuie sa dea pentru resursele lipsa (excludem banutii pentru ca ei nu fac parte din logica de comert)
std::uint8_t Player::calculateTradeCost(const std::map<Resource, std::uint8_t>& requiredResources, const Player& opponent) const {

	const auto missingResources = this->MissingResources(requiredResources, opponent);
	std::uint8_t totalTradeCost = 0;

	for (const auto& [res, missingAmount] : missingResources) {
		if (res == Resource::Coin) {
			continue;
		}
		std::uint8_t costPerUnit = this->findUnitTradeCost(res, opponent);
		totalTradeCost += missingAmount * costPerUnit;
	}

	return totalTradeCost;
}


template<typename T>
std::map<Resource, std::uint8_t> Player::findMissingResources(const T& buildable, const Player& opponent) const {
	return MissingResources(buildable.getCost(), opponent);
}

template<typename T>
std::uint8_t Player::findTradeCost(const T& buildable, const Player& opponent) const {
	return calculateTradeCost(buildable.getCost(), opponent);
}


//Functie care returneaza costul pentru  a cumpara cartea 
template<typename T>
std::uint8_t Player::findTotalCost(const T& buildable, const Player& opponent) const {
	// Verifică dacă buildable are chain symbol și dacă îl avem (doar pentru CardBase)
	if constexpr (std::is_same_v<T, CardBase>) {
		if (buildable.m_unlocks.has_value() && m_chainSymbols.count(buildable.m_unlocks.value())) {
			return 0; // Costul este 0 (Gratuit prin chain)
		}
	}

	std::uint8_t totalCost = this->getTradeCost(buildable, opponent) + buildable.getCostForResource(Resource::Coin);

	if (m_Resources.count(Resource::Coin) && m_Resources.at(Resource::Coin) >= totalCost) {
		return totalCost;
	}

	return static_cast<std::uint8_t>(-1); // 255 = imposibil
}



const std::map<Color, std::vector<std::shared_ptr<CardBase>>>& Player::getInventory() const {
	return m_Inventory;
}
const std::vector<Wonder>& Player::getWonders() const {
	return m_Wonders;
}

//Functie care cumpara cartea 
bool Player::buyCard(std::shared_ptr<CardBase> card, const Player& opponent, const Board& board) {
	std::uint8_t totalCoinCost = this->findTotalCost(*card, opponent);
	if (totalCoinCost == 0) {
		std::cout << "Card " << card->getName() << " is free." << std::endl;
	}
	if (totalCoinCost == static_cast<std::uint8_t>(-1)) {
		std::cout << "Card " << card->getName() << " can't be bought: Insufficient funds or resource trading cost is too high." << std::endl;
		return false;
	}
	if (totalCoinCost > 0) {
		m_Resources[Resource::Coin] -= totalCoinCost;
	}

	m_Inventory[card->m_color].push_back(card);
	card->applyEffect(*this, opponent, board);
	std::cout << "Card " << card->getName() << " constructed successfully. Cost paid: " << totalCoinCost << " coins." << std::endl;
	return true;

}

// gestionare inventar
void Player::addCardToInventory(std::shared_ptr<CardBase> card) {
	m_Inventory[card->m_color].push_back(card);
}

bool Player::removeCardFromInventory(std::shared_ptr<CardBase> card) {
	auto& cards = m_Inventory[card->m_color];
	for (auto it = cards.begin(); it != cards.end(); ++it) {
		if ((*it)->getId() == card->getId()) {
			cards.erase(it);
			return true;
		}
	}
	return false;
}

//Functie in cazul in care jucatorul alege sa arda cartea pentru banuti
void Player::discardCard(const CardBase& card) {

	std::uint8_t gainedCoins = GameConstants::DISCARD_BASE_COINS;
	gainedCoins += static_cast<uint8_t>(m_Inventory[Color::Yellow].size()) * GameConstants::DISCARD_YELLOW_BONUS;
	addCoins(gainedCoins);
}

void Player::constructWonder(std::shared_ptr<CardBase> cardUsed, Wonder& wonderToBuild, Player& opponent, Board& board) {
	std::uint8_t totalCoinCost = this->findTotalCost(wonderToBuild, opponent); // TEMPLATE!

	if (totalCoinCost == static_cast<std::uint8_t>(-1)) {
		std::cout << "Wonder " << wonderToBuild.getName() << " can't be built: Insufficient funds or resource trading cost is too high." << std::endl;
		return;
	}

	if (totalCoinCost > 0) {
		m_Resources[Resource::Coin] -= totalCoinCost;
	}

	// Marcăm wonder ca built
	wonderToBuild.setIsBuilt();

	// Aplicăm efectul de la wonder
	for (const auto& effect : wonderToBuild.getEffects()) {
		effect(*this, opponent);
	}

	// Discard la cartea care o folosim să activăm wonder
	board.addCardToDiscardPile(cardUsed);

	std::cout << "Wonder " << wonderToBuild.getName() << " constructed successfully. Cost paid: " << static_cast<int>(totalCoinCost) << " coins." << std::endl;
}

std::uint8_t Player::getScientificSymbolCount() const {
	
	return static_cast<std::uint8_t>(m_scientificSymbols.size());
}

std::uint8_t Player::getVPFromMilitaryTokens() const {
	
	if (m_pointsScore.count(Points::Military)) {
		return m_pointsScore.at(Points::Military);
	}
	return 0;
}


//
//std::uint8_t Player::getVPFromBlueCards() const {
//	std::uint8_t totalVP = 0;
//
//	if (m_Inventory.count(Color::Blue)) {
//		for (const auto& card : m_Inventory.at(Color::Blue)) {
//			
//			totalVP += card->getVP();
//		}
//	}
//	return totalVP;
//}


//std::uint8_t Player::getVPFromWonders() const {
//	std::uint8_t totalVP = 0;
//
//	for (const auto& wonder : m_Wonders) {
//		
//		totalVP += wonder.getVictoryPoints();
//	}
//
//	return totalVP;
//}





template std::map<Resource, std::uint8_t> Player::findMissingResources<CardBase>(const CardBase&, const Player&) const;
template std::map<Resource, std::uint8_t> Player::findMissingResources<Wonder>(const Wonder&, const Player&) const;
template std::uint8_t Player::findTradeCost<CardBase>(const CardBase&, const Player&) const;
template std::uint8_t Player::findTradeCost<Wonder>(const Wonder&, const Player&) const;
template std::uint8_t Player::findTotalCost<CardBase>(const CardBase&, const Player&) const;
template std::uint8_t Player::findTotalCost<Wonder>(const Wonder&, const Player&) const;








