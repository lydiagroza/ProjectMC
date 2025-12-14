#pragma one
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
#include<cstdint>
#include "gameExport.h"   

//vreau functie care sa im ireturneze cate simboluri stintifice are un player
//fucntie care imi retuneaza cati banuti are un player
//uramtoare le fucntii .getVPFromBuildings();
					//getVPFromWonders();
					//getVPFromTokens();
					//getVPFromBlueCards();
class GAME_API Player {
private:
	std::string m_name;
	std::map<Resource, std::uint8_t> m_Resources; // All resources
	std::map<Points, std::uint8_t>m_pointsScore; // All points
	// 0 - Wood , 1-Clay , 2- Stone, 3- Glass, 4-Papyrus
	std::bitset<5>m_discountedResource; // discounted resources from yellow cards
	std::vector<std::bitset<5>>m_flexibleResources;
	std::map<Color, std::vector<std::shared_ptr<CardBase>>>m_Inventory; // all cards
	std::vector<Wonder>m_Wonders; // all wonders
	std::unordered_set<Scientific_Symbol>m_scientificSymbols; // all scientific symbols
	bool m_hasExtraTurn = false;
	std::unordered_set<Symbol>m_chainSymbols; // all chain symbols accumulated
public:
	
	//Getteri
	std::string getName() const;

	const std::map<Resource, std::uint8_t>& getResources() const;
	
	const std::map<Points, std::uint8_t>& getPoints() const;
	
	const std::map<Color, std::vector<std::shared_ptr<CardBase>>>& getInventory() const;
	
	const std::vector<Wonder>& getWonders() const;
	
	const std::unordered_set<Scientific_Symbol>& getScientificSymbols() const;
	
	bool hasExtraTurn() const;

	const std::unordered_set<Symbol>& getChainSymbols() const;

	 std::uint8_t getCoins() ;

	
	
	//Setteri
	void set_discountedResource(Resource r);
	Player(const std::string& playerName); // constructor
	//Gestioneaza monede
	bool decreaseCoins(std::uint8_t amount); //
	void addCoins(std::uint8_t amount); //
	//Gestioneaza resursele
	void add_Resource(Resource r, std::uint8_t amount);//
	void removeResource(Resource r, std::uint8_t amount);
	void add_FlexibleResource(const std::vector<Resource>& options);
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
	bool buyCard(std::shared_ptr<CardBase> card, const Player& opponent, const Board& board);
	void discardCard(const CardBase& c);//
	void constructWonder(std::shared_ptr<CardBase> cardUsed, Wonder& wonderToBuild, Player& opponent, Board& board);
	//chestii pentru gestionare inventar
	void addCardToInventory(std::shared_ptr<CardBase> card);
	bool removeCardFromInventory(std::shared_ptr<CardBase> card);
	
	//Numarul de simboluri stiintifice
	std::uint8_t getScientificSymbolCount() const;

	//Puncte din tokens
	std::uint8_t getVPFromTokens() const;

	//Puncte din albastre
	std::uint8_t getVPFromBlueCards() const;

private:
	std::map<Resource, std::uint8_t> MissingResources(const std::map<Resource, std::uint8_t>& requiredResources, const Player& opponent) const;
	std::uint8_t calculateTradeCost(const std::map<Resource, std::uint8_t>& requiredResources, const Player& opponent) const;
	int findResourceDiscountIndex(Resource r) const;
	Resource findResourceDiscountFromIndex(int index) const;
};
extern template std::map<Resource, std::uint8_t> Player::findMissingResources<CardBase>(const CardBase&, const Player&) const;
extern template std::map<Resource, std::uint8_t> Player::findMissingResources<Wonder>(const Wonder&, const Player&) const;
extern template std::uint8_t Player::findTradeCost<CardBase>(const CardBase&, const Player&) const;
extern template std::uint8_t Player::findTradeCost<Wonder>(const Wonder&, const Player&) const;
extern template std::uint8_t Player::findTotalCost<CardBase>(const CardBase&, const Player&) const;
extern template std::uint8_t Player::findTotalCost<Wonder>(const Wonder&, const Player&) const;
