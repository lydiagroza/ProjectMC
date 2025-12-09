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
class Player {
private:
	std::string m_name;
	std::map<Resource, std::uint8_t> m_Resources; // All resources
	std::map<Points, std::uint8_t>m_pointsScore; // All points
	// 0 - Wood , 1-Clay , 2- Stone, 3- Glass, 4-Papyrus
	std::bitset<5>m_discountedResource; // discounted resources from yellow cards
	std::map<std::string, std::vector<Resource>>m_flexibleResources; // vector,bitset> 
	std::map<Color, std::vector<std::shared_ptr<CardBase>>>m_Inventory; // all cards
	std::vector<Wonder>m_Wonders; // all wonders
	std::unordered_set<Scientific_Symbol>m_scientificSymbols; // all scientific symbols
	bool m_hasExtraTurn = false;
	std::unordered_set<Symbol>m_chainSymbols; // all chain symbols accumulated
public:
	// getteri
	const std::map<Color, std::vector<std::shared_ptr<CardBase>>>& getInventory() const;
	const std::vector<Wonder>& getWonders() const;
	void set_discountedResource(Resource r);
	Player(const std::string& playerName); // constructor
	//Gestioneaza monede
	bool decreaseCoins(std::uint8_t amount); //
	void addCoins(std::uint8_t amount); //
	//Gestioneaza resursele
	void add_Resource(Resource r, std::uint8_t amount);//
	void removeResource(Resource r, std::uint8_t amount);
	//Gestioneaza punctele de victorie
	void add_Points(Points p, std::uint8_t amount);//
	//Gestioneaza simbolurile
	void add_ScientificSymbol(Scientific_Symbol symbol);//
	void add_ChainSymbol(Symbol symbol);//
	//Functie ca sa vad cat ma costa o singura resursa
	std::uint8_t getUnitTradeCost(Resource res, const Player& opponent) const;
	//Functie pentru a afla toate alegerile de resurse
	std::vector<std::vector<Resource>> getFlexibleChoices() const; // ---> de modif 
	//Functie pentru a afla ce resurse ne lipsesc pentru a cumpara o carte
	template<typename T>
	std::map<Resource, std::uint8_t> getMissingResources(const T& buildable, const Player& opponent) const;
	//Functie care ne zice cati banuti trebuie sa dea pentru resursele lipsa
	template<typename T>
	std::uint8_t getTradeCost(const T& buildable, const Player& opponent)const;
	//Functie care verifica daca putem cumpara cartea
	template<typename T>
	std::uint8_t getTotalCost(const T& buildable, const Player& opponent)const;
	std::string getName() const;
	//Actiuni de joc
	bool hasExtraTurn() const { return m_hasExtraTurn; }
	bool buyCard(std::shared_ptr<CardBase> card, const Player& opponent, const Board& board);
	void discardCard(const CardBase& c);//
	void constructWonder(std::shared_ptr<CardBase> cardUsed, Wonder& wonderToBuild, Player& opponent, Board& board);
	//chestii pentru gestionare inventar
	void addCardToInventory(std::shared_ptr<CardBase> card);
	bool removeCardFromInventory(std::shared_ptr<CardBase> card);
private:
	std::map<Resource, std::uint8_t> MissingResources(const std::map<Resource, std::uint8_t>& requiredResources, const Player& opponent) const;
	std::uint8_t calculateTradeCost(const std::map<Resource, std::uint8_t>& requiredResources, const Player& opponent) const;
	int getResourceDiscountIndex(Resource r) const;
};
extern template std::map<Resource, std::uint8_t> Player::getMissingResources<CardBase>(const CardBase&, const Player&) const;
extern template std::map<Resource, std::uint8_t> Player::getMissingResources<Wonder>(const Wonder&, const Player&) const;
extern template std::uint8_t Player::getTradeCost<CardBase>(const CardBase&, const Player&) const;
extern template std::uint8_t Player::getTradeCost<Wonder>(const Wonder&, const Player&) const;
extern template std::uint8_t Player::getTotalCost<CardBase>(const CardBase&, const Player&) const;
extern template std::uint8_t Player::getTotalCost<Wonder>(const Wonder&, const Player&) const;
