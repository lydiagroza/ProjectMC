#pragma once
#include<string>
#include<memory>
#include<unordered_map>
#include<map>
#include<set>
#include<vector>
#include <bitset>
#include<type_traits>
#include<unordered_set>
#include "CardBase.h"
#include "Board.h"
#include "Wonder.h"
#include "ProgressTokens.h"
#include<cstdint>
#include "gameExport.h"   


class GAME_API Player {
private:
	std::string m_name;
	std::map<Resource, std::uint8_t> m_Resources; // All resources
	std::map<Points, std::uint8_t>m_pointsScore; // All points
	std::bitset<5>m_discountedResource; // discounted resources from yellow cards
	std::vector<std::bitset<5>>m_flexibleResources; 
	std::map<Color, std::vector<std::shared_ptr<CardBase>>>m_Inventory; // all cards
	std::vector<Wonder>m_Wonders; // all wonders
	std::unordered_set<Scientific_Symbol>m_scientificSymbols; // all scientific symbols
	bool m_hasExtraTurn = false;
	std::unordered_set<Symbol>m_chainSymbols; // all chain symbols accumulated

	// Progress Token Effects
	bool m_hasWonderDiscount = false;
	bool m_hasBlueCardDiscount = false;
	bool m_gainsOpponentTradeCost = false;
	bool m_getsCoinsForFreeCards = false;
	std::vector<std::shared_ptr<ProgressToken>> m_progressTokens;
public:
	// getteri
	std::string getName() const;

	const std::map<Resource, std::uint8_t>& getResources() const;

	const std::map<Points, std::uint8_t>& getPoints() const;

	const std::map<Color, std::vector<std::shared_ptr<CardBase>>>& getInventory() const;

	const std::vector<Wonder>& getWonders() const;

	const std::unordered_set<Scientific_Symbol>& getScientificSymbols() const;

	bool hasExtraTurn() const;

	const std::unordered_set<Symbol>& getChainSymbols() const;

	int getCoins() const;

	int getNrOfScientificSymbols() const;

	int getVPFromMilitaryTokens() const;

	int getVPFromBlueCards() const;

	




	void setDiscountedResource(Resource r);
	Player(const std::string& playerName); // constructor
	//Gestioneaza monede
	bool decreaseCoins(std::uint8_t amount); //
	//Gestioneaza resursele
	void addResource(Resource r, std::uint8_t amount);//
	void removeResource(Resource r, std::uint8_t amount);
	//Gestioneaza punctele de victorie
	void addPoints(Points p, std::uint8_t amount);//
	//Gestioneaza simbolurile
	void addScientificSymbol(Scientific_Symbol symbol);//
	void addChainSymbol(Symbol symbol);//
	//Functie ca sa vad cat ma costa o singura resursa
	std::uint8_t findUnitTradeCost(Resource res, const Player& opponent) const;
	//Functie pentru a afla toate alegerile de resurse
	std::vector<std::vector<Resource>> findFlexibleChoices() const; // ---> de modif 
	//Functie pentru a afla ce resurse ne lipsesc pentru a cumpara o carte
	template<typename T>
	std::map<Resource, std::uint8_t> findMissingResources(const T& buildable, const Player& opponent) const;
	//Functie care ne zice cati banuti trebuie sa dea pentru resursele lipsa
	template<typename T>
	std::uint8_t findTradeCost(const T& buildable, const Player& opponent)const;
	//Functie care verifica daca putem cumpara cartea
	template<typename T>
	std::uint8_t findTotalCost(const T& buildable, const Player& opponent)const;
	
	//Actiuni de joc

	bool buyCard(std::shared_ptr<CardBase> card, const Player& opponent, const Board& board);

	bool buyCard(std::shared_ptr<CardBase> card, Player& opponent, const Board& board);
	void discardCard(const CardBase& c);
	void constructWonder(std::shared_ptr<CardBase> cardUsed, Wonder& wonderToBuild, Player& opponent, Board& board);
	//chestii pentru gestionare inventar
	void addCardToInventory(std::shared_ptr<CardBase> card);
	bool removeCardFromInventory(std::shared_ptr<CardBase> card);

	void addWonders(const std::shared_ptr<Wonder>& wonder);
	

	// Progress Token Effects
	void setWonderDiscount(bool v);
	void setBlueCardDiscount(bool v);
	void setGainsOpponentTradeCost(bool v);
	void setGetsCoinsForFreeCards(bool v);
	bool hasWonderDiscount();
	bool hasBlueCardDiscount();	
	bool gainsOpponentTradeCost();
	bool getsCoinsForFreeCards();
	void addProgressToken(std::shared_ptr<ProgressToken> token);
	const std::vector<std::shared_ptr<ProgressToken>>& getProgressTokens();
	void setHasExtraTurn(bool v);

private:
	std::map<Resource, std::uint8_t> MissingResources(const std::map<Resource, std::uint8_t>& requiredResources, const Player& opponent) const;
	std::uint8_t calculateTradeCost(const std::map<Resource, std::uint8_t>& requiredResources, const Player& opponent) const;
	int getResourceDiscountIndex(Resource r) const;
	Resource findResourceDiscountFromIndex(int index) const;
};

extern template std::uint8_t Player::findTradeCost<CardBase>(const CardBase&, const Player&) const;
extern template std::uint8_t Player::findTradeCost<Wonder>(const Wonder&, const Player&) const;
extern template std::uint8_t Player::findTotalCost<CardBase>(const CardBase&, const Player&) const;
extern template std::uint8_t Player::findTotalCost<Wonder>(const Wonder&, const Player&) const;
