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
#include "GameConstants.h"


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
	std::unordered_map<Scientific_Symbol, int> m_scientificSymbols; // all scientific symbols
	std::unordered_set<Symbol>m_chainSymbols; // all chain symbols accumulated
	enum PlayerFlags {
		ExtraTurn = 0,
		WonderDiscount = 1,
		BlueCardDiscount = 2,
		OpponentTrade = 3,
		FreeCardCoins = 4,
		ExtraMilitary = 5,
		MathBonus=6,
		TheologyBonus=7,
		FlagCount = 8
	};
	std::bitset<FlagCount> m_flags;
	std::vector<std::shared_ptr<ProgressToken>> m_progressTokens;

	std::uint8_t findUnitTradeCost(Resource r, const Player& opponent) const;
	std::uint8_t calculateTradeCost(const std::map<Resource, std::uint8_t>& requiredResources, const Player& opponent) const;
	int getResourceDiscountIndex(Resource r) const;
	Resource findResourceDiscountFromIndex(int index) const;

	template<typename T>
	std::uint8_t findTradeCost(const T& buildable, const Player& opponent) const {
		return calculateTradeCost(buildable.getCost(), opponent);
	}



public:
	// getteri
	unsigned int getId() const;
	std::string getName() const;
    void setName(const std::string& name) { m_name = name; }
	std::vector<std::bitset<5>> getChoiceResources() const;	
	const std::map<Resource, std::uint8_t>& getResources() const;
	const std::map<Points, std::uint8_t>& getPoints() const;

	const std::map<Color, std::vector<std::shared_ptr<CardBase>>>& getInventory() const;

	const std::vector<std::shared_ptr<Wonder>>& getWonders() const;

	const std::unordered_map<Scientific_Symbol,int>& getScientificSymbols() const;


	const std::unordered_set<Symbol>& getChainSymbols() const;

	int getCoins() const;

	int getNrOfScientificSymbols() const;
	const std::vector<std::shared_ptr<ProgressToken>>& getProgressTokens() const;


	//VP for winner determination
	int getVPFromMilitaryTokens() const;
	int getVPFromBlueCards() const;
	int getVPFromGuilds(const Player& opponent) const;

	void addChoiceResources(std::vector<Resource> choices);

	void set_discountedResource(Resource r);
	Player(const std::string& playerName,int id); // constructor
	Player(const std::string& name);
    virtual ~Player() = default; // Added to make class polymorphic
	//Gestioneaza monede
	bool decreaseCoins(std::uint8_t amount); //
	//Gestioneaza resursele
	void addResource(Resource r, std::uint8_t amount);//
	void removeResource(Resource r, std::uint8_t amount);
	//Gestioneaza punctele de victorie
	void add_Points(Points p, std::uint8_t amount);//
	//Gestioneaza simbolurile
	bool add_ScientificSymbol(Scientific_Symbol symbol);//
	int getUniqueScientificSymbolsCount() const;
	void add_ChainSymbol(Symbol symbol);//

	
	std::map<Resource, std::uint8_t> MissingResources(const std::map<Resource, std::uint8_t>& requiredResources,const Player& opponent) const;



	template<typename T>
	std::uint8_t findTotalCost(const T& buildable, const Player& opponent) const {
		// Verific? dac? buildable are chain symbol ?i dac? �l avem (doar pentru CardBase)
		if constexpr (std::is_same_v<T, CardBase>) {
			if (buildable.m_unlocks.has_value() && m_chainSymbols.count(buildable.m_unlocks.value())) {
				return 0; // Costul este 0 (Gratuit prin chain)
			}
		}

		std::uint8_t totalCost = this->findTradeCost(buildable, opponent) + buildable.getCostForResource(Resource::Coin);

		// Apply discounts from Progress Tokens
		if constexpr (std::is_same_v<T, Wonder>) {
			if (hasWonderDiscount() && totalCost > 0) {
				totalCost = (totalCost > 1) ? totalCost - 1 : 0;
			}
		}
		if constexpr (std::is_same_v<T, CardBase>) {
			if (hasBlueCardDiscount() && buildable.getColor() == Color::Blue && totalCost > 0) {
				totalCost = (totalCost > 1) ? totalCost - 1 : 0;
			}
		}

		uint8_t availableCoins = m_Resources.count(Resource::Coin) ? m_Resources.at(Resource::Coin) : 0;
		if (availableCoins >= totalCost) {
			return totalCost;
		}

		return GameConstants::IMPOSSIBLE_COST;
	}
	
	//Actiuni de joc

	bool buyCard(std::shared_ptr<CardBase> card,  Player& opponent,  Board& board);

	void discardCard(const CardBase& c);
	bool constructWonder(std::shared_ptr<CardBase> cardUsed, Wonder& wonderToBuild, Player& opponent, Board& board);
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

	inline bool hasExtraMilitary() const { return m_flags.test(ExtraMilitary); }
	inline void setExtraMilitary(bool v) { m_flags.set(ExtraMilitary, v); }

	inline bool hasMathBonus() const { return m_flags.test(MathBonus); }
	inline void setMathBonus(bool v) { m_flags.set(MathBonus, v); }

	inline bool hasTheologyBonus() const { return m_flags.test(TheologyBonus); }
	inline void setTheologyBonus(bool v) { m_flags.set(TheologyBonus, v); }

	void addProgressToken(std::shared_ptr<ProgressToken> token);
	const std::vector<std::shared_ptr<ProgressToken>>& getProgressTokens();

	

};

