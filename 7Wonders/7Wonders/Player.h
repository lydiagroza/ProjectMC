#include<string>
#include<unordered_map>
#include<map>
#include<vector>
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
	std::map<Resource, std::uint8_t>m_discountedResource; // discounted resources from yellow cards
	std::map<Color, std::vector<CardBase>>m_Inventory; // all cards 
	std::vector<Wonder>m_Wonders; // all wonders
	std::unordered_set<Scientific_Symbol>m_scientificSymbols; // all scientific symbols 
	std::unordered_set<Symbol>m_chainSymbols; // all chain symbols accumulated
public:
	Player(const std::string& playerName); // constructor 
	
	//Gestioneaza monede
	bool decreaseCoins(std::uint8_t amount); //
	void addCoins(std::uint8_t amount); //

	//Gestioneaza resursele
	void add_Resource(Resource r, std::uint8_t amount);//
	void add_DiscountedResource(Resource r, std::uint8_t discount);

	//Gestioneaza punctele de victorie
	void add_Points(Points p, std::uint8_t amount);//

	//Gestioneaza simbolurile
	void add_ScientificSymbol(Scientific_Symbol symbol);//
	void add_ChainSymbol(Symbol symbol);//

	//Functie ca sa extrag resursele de baza fara coins
	std::map<Resource, std::uint8_t>getBaseProduction() const;

	//Functie ca sa vad cat ma costa o singura resursa
	std::uint8_t getUnitTradeCost(Resource res, const Player& opponent) const;

	//Functie pentru a afla toate alegerile de resurse 
	std::vector<std::vector<Resource>> getFlexibleChoices() const;

	//Functie pentru a afla ce resurse ne lipsesc pentru a cumpara o carte
	std::map<Resource, std::uint8_t> getMissingResources(const CardBase& card, const Player& opponent) const;


	//Functie care ne zice cati banuti trebuie sa dea pentru resursele lipsa
	std::uint8_t calculateTradeCost(const CardBase& card, const Player& opponent)const; 

	//Functie care verifica daca putem cumpara cartea
	bool canAffordConstruction(const CardBase& card, const Player& opponent);

	//Actiuni de joc
	bool buyCard(const CardBase& card, const Player& opponent, const Board& board);
	void discardCard(const CardBase& c);//
	void constructWonder(const CardBase& cardUsed, const Wonder& wonderToBuild, Player& opponent, Board& board);
};
