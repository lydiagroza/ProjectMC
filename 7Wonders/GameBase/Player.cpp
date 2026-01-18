#include "Player.h"
#include "GameConstants.h"
#include "GuildEffects.h"
#include <iostream>
#include <algorithm> // For std::max

//Constructor
Player::Player(const std::string& playerName, int id)
    : 
    m_Id(id),
    m_name(playerName),
    m_Resources{},
    m_pointsScore{},
    m_discountedResource{},
    m_choiceResources{},
    m_Inventory{},
    m_Wonders{},
    m_scientificSymbols{},
    m_chainSymbols{},
    m_flags(0),
    m_progressTokens{}
{
}

unsigned int Player::getId() const
{
    return m_Id;
}

//Getter function for player name
std::string Player::getName()const {
    return m_name;
}

const std::map<Resource, std::uint8_t>& Player::getResources() const {
    return m_Resources;
}

const std::map<Color, std::vector<std::shared_ptr<CardBase>>>& Player::getInventory() const {
    return m_Inventory;
}

std::vector<std::bitset<5>> Player::getChoiceResources() const
{
    return m_choiceResources;
}

const std::map<Points, std::uint8_t>& Player::getPoints() const
{
    return m_pointsScore;
}

const std::unordered_map<Scientific_Symbol, int>& Player::getScientificSymbols() const {
    return m_scientificSymbols;

}

const std::unordered_set<Symbol>& Player::getChainSymbols() const {
    return m_chainSymbols;

}

const std::vector<std::shared_ptr<ProgressToken>>& Player::getProgressTokens() const
{
    return m_progressTokens;
}

int Player::getUniqueScientificSymbolsCount() const {
    // For Scientific Supremacy victory (6 DIFFERENT symbols)
    return static_cast<int>(m_scientificSymbols.size());
}


int Player::getCoins() const
{
    auto it = m_Resources.find(Resource::Coin);
    return (it != m_Resources.end()) ? it->second : 0;
}


const std::vector<std::shared_ptr<Wonder>>& Player::getWonders() const {
    return m_Wonders;
}

int Player::getNrOfScientificSymbols() const
{
    return m_scientificSymbols.size();
}

int Player::getVPFromMilitaryTokens() const {

    if (m_pointsScore.count(Points::Military)) {
        return m_pointsScore.at(Points::Military);
    }
    return 0;
}

int Player::getVPFromBlueCards() const {
    if (m_pointsScore.count(Points::BlueCards)) {
        return m_pointsScore.at(Points::BlueCards);
    }

    return 0;
}



// Functions for managing coins
bool Player::decreaseCoins(std::uint8_t amount) {
    if (m_Resources[Coin] >= amount) {
        m_Resources[Coin] -= amount;
        return true;
    }
    std::cout << "Insufficient coins";
    return false;
}

void Player::addResource(Resource r, std::uint8_t amount)
{
    m_Resources[r] += amount;
}

void Player::set_discountedResource(Resource r) {
    int index = getResourceDiscountIndex(r);
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

// Functions for points and symbols
void Player::add_Points(Points p, std::uint8_t amount)
{
    m_pointsScore[p] += amount;
}

bool Player::add_ScientificSymbol(Scientific_Symbol symbol) {
    m_scientificSymbols[symbol]++;
    if (m_scientificSymbols[symbol] % 2 == 0) {
        return true; 
    }
    return false;
}


void Player::add_ChainSymbol(Symbol symbol)
{
    m_chainSymbols.insert(symbol);
}



void Player::addChoiceResources(std::vector<Resource> choices)



{



    std::bitset<5> all_choices;



    for (int i = 0; i < (int)choices.size(); i++) {



        all_choices[getResourceDiscountIndex(choices[i])] = 1;



    }



    m_choiceResources.push_back(all_choices);



}




int Player::getResourceDiscountIndex(Resource r) const {
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


// Function to find the cost of a single resource that the player needs  
std::uint8_t Player::findUnitTradeCost(Resource r, const Player& opponent) const {

    int index = getResourceDiscountIndex(r);
    if (index != -1 && m_discountedResource[index] == 1) {
        return 1;
    }

    std::uint8_t opponentResourceProduction = opponent.m_Resources.count(r) ? opponent.m_Resources.at(r) : 0;

    return GameConstants::TRADE_BASE_COST + opponentResourceProduction;
}




// Function that returns a map with missing resources for a card 
std::map<Resource, std::uint8_t> Player::MissingResources(
    const std::map<Resource, std::uint8_t>& requiredResources,
    const Player& opponent) const {

    std::map<Resource, std::uint8_t> missingResources;

    // Calculate missing resources (UNCHANGED)
    for (const auto& [res, reqAmount] : requiredResources) {
        if (res == Resource::Coin) continue;

        uint8_t availableResource = m_Resources.count(res) ? m_Resources.at(res) : 0;

        if (availableResource < reqAmount) {
            missingResources[res] = reqAmount - availableResource;
        }
    }


    for (const auto& flexibleSet : m_choiceResources) {
    

        Resource bestResourceToCover = Resource::Coin;
        std::uint8_t maxCost = 0;

    
        for (int i = 0; i < 5; ++i) {
            if (flexibleSet[i] == 0) continue; // Bit = 0 -> resource not available


            Resource resToCover = findResourceDiscountFromIndex(i);


            if (missingResources.count(resToCover) && missingResources.at(resToCover) > 0) {
                std::uint8_t currentCost = this->findUnitTradeCost(resToCover, opponent);


                if (currentCost > maxCost) {
                    maxCost = currentCost;
                    bestResourceToCover = resToCover;
                }
            }
        }


        if (bestResourceToCover != Resource::Coin) {
            missingResources.at(bestResourceToCover)--;

            if (missingResources.at(bestResourceToCover) == 0) {
                missingResources.erase(bestResourceToCover);
            }
        }
    }
    return missingResources; 
}

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

bool Player::buyCard(std::shared_ptr<CardBase> card, Player& opponent, Board& board) {

    std::uint8_t totalCoinCost = this->findTotalCost(*card, opponent);
    if (totalCoinCost == 0) {
        std::cout << "Card " << card->getName() << " is free." << std::endl;
        if (getsCoinsForFreeCards()) { // Check if player gets coins for free cards
            addResource(Coin,4); // Assuming 4 coins for free card
            std::cout << getName() << " gained 4 coins for building " << card->getName() << " for free.\n";
        }
    }
    if (totalCoinCost == GameConstants::IMPOSSIBLE_COST) {
        std::cout << "Card " << card->getName() << " can't be bought: Insufficient funds or resource trading cost is too high." << std::endl;
        return false;
    }
    if (totalCoinCost > 0) {
        // Calculate trade cost separately to give to opponent if needed
        std::uint8_t tradeCost = this->findTradeCost(*card, opponent);
        if (opponent.gainsOpponentTradeCost()) { // If opponent has the token, they gain coins
            opponent.addResource(Coin,tradeCost); // Opponent gains coins
            std::cout << opponent.getName() << " gained " << static_cast<int>(tradeCost) << " coins from " << getName() << "'s trade.\n";
        }

        m_Resources[Resource::Coin] -= totalCoinCost;
    }

    m_Inventory[card->m_color].push_back(card);
    card->applyEffect(*this,board,opponent);
    if (card->getColor() == Color::Red && hasExtraMilitary()) {
        int lostCoins = 0;
        board.getMilitaryTrack().applyShields(this->getId(), 1, lostCoins);
        if (lostCoins > 0) {
            opponent.decreaseCoins(lostCoins);
            std::cout << opponent.getName() << " lost " << lostCoins << " coins due to military tokens.\n";
        }
        add_Points(Points::Military, 1);
        std::cout << "[Strategy Bonus] +1 shield applied!\n";
    }
    std::cout << "Card " << card->getName() << " constructed successfully. Cost paid: " << totalCoinCost << " coins." << std::endl;
    return true;

}

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

void Player::discardCard(std::shared_ptr<CardBase> c, Board& board) {

    std::uint8_t gainedCoins = GameConstants::DISCARD_BASE_COINS;
    gainedCoins += static_cast<uint8_t>(m_Inventory[Color::Yellow].size()) * GameConstants::DISCARD_YELLOW_BONUS;
    addResource(Coin,gainedCoins);
    board.addCardToDiscardPile(c);
}

bool Player::constructWonder(std::shared_ptr<CardBase> cardUsed, Wonder &wonderToBuild, Player& opponent, Board& board) {
    std::uint8_t totalCoinCost = this->findTotalCost(wonderToBuild, opponent); // TEMPLATE! 

    if (totalCoinCost == static_cast<std::uint8_t>(-1)) {
        std::cout << "Wonder " << wonderToBuild.getName() << " can't be built: Insufficient funds or resource trading cost is too high." << std::endl;
        return false;
    }

    if (totalCoinCost > 0) {
        // Calculate trade cost separately to give to opponent if needed
        std::uint8_t tradeCost = this->findTradeCost(wonderToBuild, opponent);
        if (opponent.gainsOpponentTradeCost()) { // If opponent has the token, they gain coins
            opponent.addResource(Coin,tradeCost); // Opponent gains coins
            std::cout << opponent.getName() << " gained " << static_cast<int>(tradeCost) << " coins from " << getName() << "'s wonder construction trade.\n";
        }
        m_Resources[Resource::Coin] -= totalCoinCost;
    }

    // Mark wonder as built
    wonderToBuild.setIsBuilt();

    // Apply wonder effect
    for (const auto& effect : wonderToBuild.getEffects()) {
        effect(*this, board,opponent);
    }

    if (hasTheologyBonus()) {
        setHasExtraTurn(true);
        std::cout << "[Theology] gave an extra round bonus!\n";
    }

    // Discard used card
    board.addCardToDiscardPile(cardUsed);

    std::cout << "Wonder " << wonderToBuild.getName() << " constructed successfully. Cost paid: " << static_cast<int>(totalCoinCost) << " coins." << std::endl;
    return true;
}

void Player::addWonders(const std::shared_ptr<Wonder>& wonder)
{
    m_Wonders.push_back(wonder);
}


void Player::addProgressToken(std::shared_ptr<ProgressToken> token) {
    m_progressTokens.push_back(token);
}

int Player::getVPFromGuilds(const Player& opponent) const {
    return GuildEndGameEffects::calculateTotalGuildVP(*this, opponent);
}
