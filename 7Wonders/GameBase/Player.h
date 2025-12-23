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
	unsigned int m_Id:2;
	std::string m_name;
	std::map<Resource, std::uint8_t> m_Resources; // All resources
	std::map<Points, std::uint8_t>m_pointsScore; // All points
	std::bitset<5>m_discountedResource; // discounted resources from yellow cards
	std::vector<std::bitset<5>>m_choiceResources; 
	std::map<Color, std::vector<std::shared_ptr<CardBase>>>m_Inventory; // all cards
	std::vector<std::shared_ptr<Wonder>> m_Wonders; // all wonders
	std::unordered_set<Scientific_Symbol>m_scientificSymbols; // all scientific symbol
	std::unordered_set<Symbol>m_chainSymbols; // all chain symbols accumulated
	enum PlayerFlags {
		ExtraTurn = 0,
		WonderDiscount = 1,
		BlueCardDiscount = 2,
		OpponentTrade = 3,
		FreeCardCoins = 4,
		FlagCount = 5
	};
	std::bitset<FlagCount> m_flags;
	std::vector<std::shared_ptr<ProgressToken>> m_progressTokens;
public:
	// getteri
	unsigned int getId() const;
	std::string getName() const;
	std::vector<std::bitset<5>> getChoiceResources() const;	
	const std::map<Resource, std::uint8_t>& getResources() const;
	const std::map<Points, std::uint8_t>& getPoints() const;

	const std::map<Color, std::vector<std::shared_ptr<CardBase>>>& getInventory() const;

	const std::vector<std::shared_ptr<Wonder>>& getWonders() const;

	const std::unordered_set<Scientific_Symbol>& getScientificSymbols() const;


	const std::unordered_set<Symbol>& getChainSymbols() const;

	int getCoins() const;

	int getNrOfScientificSymbols() const;

	int getVPFromMilitaryTokens() const;

	int getVPFromBlueCards() const;

	void addChoiceResources(std::vector<Resource> choices);

void set_discountedResource(Resource r);
	Player(const std::string& playerName,int id); // constructor
	//Gestioneaza monede
	bool decreaseCoins(std::uint8_t amount); //
	//Gestioneaza resursele
	void addResource(Resource r, std::uint8_t amount);//
	void removeResource(Resource r, std::uint8_t amount);
	//Gestioneaza punctele de victorie
	void add_Points(Points p, std::uint8_t amount);//
	//Gestioneaza simbolurile
	void add_ScientificSymbol(Scientific_Symbol symbol);//
	void add_ChainSymbol(Symbol symbol);//


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

	bool buyCard(std::shared_ptr<CardBase> card,  Player& opponent,  Board& board);

	void discardCard(const CardBase& c);
	void constructWonder(std::shared_ptr<CardBase> cardUsed, Wonder& wonderToBuild, Player& opponent, Board& board);
	//chestii pentru gestionare inventar
	void addCardToInventory(std::shared_ptr<CardBase> card);
	bool removeCardFromInventory(std::shared_ptr<CardBase> card);

	void addWonders(const std::shared_ptr<Wonder>& wonder);
	

	// Progress Token Effects
	inline bool hasExtraTurn() const { return m_flags.test(ExtraTurn); }
	inline void setHasExtraTurn(bool v) { m_flags.set(ExtraTurn, v); }

	inline bool hasWonderDiscount() const { return m_flags.test(WonderDiscount); }
	inline void setWonderDiscount(bool v) { m_flags.set(WonderDiscount, v); }

	inline bool hasBlueCardDiscount() const { return m_flags.test(BlueCardDiscount); }
	inline void setBlueCardDiscount(bool v) { m_flags.set(BlueCardDiscount, v); }

	inline bool gainsOpponentTradeCost() const { return m_flags.test(OpponentTrade); }
	inline void setGainsOpponentTradeCost(bool v) { m_flags.set(OpponentTrade, v); }

	inline bool getsCoinsForFreeCards() const { return m_flags.test(FreeCardCoins); }
	inline void setGetsCoinsForFreeCards(bool v) { m_flags.set(FreeCardCoins, v); }

	void addProgressToken(std::shared_ptr<ProgressToken> token);
	const std::vector<std::shared_ptr<ProgressToken>>& getProgressTokens();

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
