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
std::map<Resource, std::uint8_t> Player::MissingResources(const std::map<Resource, std::uint8_t>& requiredResources,const Player& opponent) const {

	std::map<Resource, std::uint8_t> missingResources;

	// Calculăm resursele lipsă
	for (const auto& [res, reqAmount] : requiredResources) {
		if (res == Resource::Coin) continue;

		uint8_t availableResource = m_Resources.count(res) ? m_Resources.at(res) : 0;

		if (availableResource < reqAmount) {
			missingResources[res] = reqAmount - availableResource;
		}
	}

	// Folosim resursele flexibile pentru a acoperi lipsurile
	for (const auto& [sourceName, choiceOptions] : m_flexibleResources) {

		Resource bestResourceToCover = Resource::Coin;
		std::uint8_t maxCost = 0;

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
std::uint8_t Player::calculateTradeCost(const std::map<Resource, std::uint8_t>& requiredResources,const Player& opponent) const {

	const auto missingResources = this->MissingResources(requiredResources, opponent);
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


template<typename T>
std::map<Resource, std::uint8_t> Player::getMissingResources(const T& buildable, const Player& opponent) const {
	return MissingResources(buildable.getCost(), opponent);
}

template<typename T>
std::uint8_t Player::getTradeCost(const T& buildable, const Player& opponent) const {
	return calculateTradeCost(buildable.getCost(), opponent);
}


//Functie care returneaza costul pentru  a cumpara cartea 
template<typename T>
std::uint8_t Player::getTotalCost(const T& buildable, const Player& opponent) const {
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

template std::map<Resource, std::uint8_t> Player::getMissingResources<CardBase>(const CardBase&, const Player&) const;
template std::map<Resource, std::uint8_t> Player::getMissingResources<Wonder>(const Wonder&, const Player&) const;

template std::uint8_t Player::getTradeCost<CardBase>(const CardBase&, const Player&) const;
template std::uint8_t Player::getTradeCost<Wonder>(const Wonder&, const Player&) const;

template std::uint8_t Player::getTotalCost<CardBase>(const CardBase&, const Player&) const;
template std::uint8_t Player::getTotalCost<Wonder>(const Wonder&, const Player&) const;




//Functie care cumpara cartea 
bool Player::buyCard(std::shared_ptr<CardBase> card, const Player& opponent, const Board &board) {
	std::uint8_t totalCoinCost = this->getTotalCost(*card, opponent);
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
	gainedCoins += m_Inventory[Color::Yellow].size() * GameConstants::DISCARD_YELLOW_BONUS;
	addCoins(gainedCoins);
}

void Player::constructWonder(std::shared_ptr<CardBase> cardUsed, Wonder& wonderToBuild, Player& opponent, Board& board) {
	std::uint8_t totalCoinCost = this->getTotalCost(wonderToBuild, opponent); // TEMPLATE!

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


//Ce este mai jos nu cred ca mai trebuie,am facut functiile template astfel putem folosi atat pentru a verifica daca isi permite cartea cat si Wonderul

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

//std::uint8_t Player::getTotalWonderCost(const Wonder& wonder, const Player& opponent) {
//    // mai trebuie getMissingResources and calculateTradeCost.
//    // momentan verificam daca player are resurse
//    std::uint8_t totalCost = 0;
//    for (const auto& [res, /*missing resources?*/] : wonder.getCost()) {
//        uint8_t availableResource = m_Resources.count(res) ? m_Resources.at(res) : 0;
//        if (availableResource < /*missing resources?*/) {
//            totalCost += (reqAmount - availableResource) * getUnitTradeCost(res, opponent);
//        }
//    }
//    
//    if (m_Resources.count(Resource::Coin) && m_Resources.at(Resource::Coin) >= totalCost) {
//        return totalCost;
//    }
//
//    return 255; // un nr random la carti parac era -1
//}
//
//void Player::constructWonder(std::shared_ptr<CardBase> cardUsed, Wonder& wonderToBuild, Player& opponent, Board& board) {
//    std::uint8_t totalCoinCost = this->getTotalWonderCost(wonderToBuild, opponent);
//
//    if (totalCoinCost == 255) {
//        std::cout << "Wonder " << wonderToBuild.getName() << " can't be built: Insufficient funds or resource trading cost is too high." << std::endl;
//        return;
//    }
//
//    if (totalCoinCost > 0) {
//        m_Resources[Resource::Coin] -= totalCoinCost;
//    }
//
//	// marcam wonder ca built
//    wonderToBuild.setIsBuilt();
//
//	// aplicam effectul de la wonder
//    for (const auto& effect : wonderToBuild.getEffects()) {
//        effect(*this, opponent);
//    }
//
//	// discard la cartea care o folosim sa activam wonder
//    board.addCardToDiscardPile(cardUsed);
//
//    std::cout << "Wonder " << wonderToBuild.getName() << " constructed successfully. Cost paid: " << totalCoinCost << " coins." << std::endl;
//}


