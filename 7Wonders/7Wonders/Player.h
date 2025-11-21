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

	// astea cu // sunt rezolvate 
	bool decreaseCoins(std::uint8_t amount);
	void addCoins(std::uint8_t amount); //
	void add_Resource(Resource r, int amount);//
	void add_Points(Points p, int amount);//
	void add_ScientificSymbol(Scientific_Symbol symbol);//
	void add_ChainSymbol(Symbol symbol);//
	bool buyCard(const CardBase& card, const Player& opponent, const Board& board);
	void add_DiscountedResource(Resource r, int discount);
	bool hasSufficientResources(const std::map<Resource, std::uint8_t>& a,
		const std::map<Resource, uint8_t>& b);//
	int calculateTradeCost(const CardBase& c, const Player& opponent)const;//calculeaza monezile necesare pentru a cumpara resursele necesare pentru a folosi o carte 
	bool canAffordConstruction(const CardBase& c, const Player& opponent);
	void discardCard(const CardBase& c);//
	void processConstruction(const CardBase& c, Player& opponent, Board& board);// trb sa nu mai fie 
	void constructWonder(const CardBase& cardUsed, const Wonder& wonderToBuild, Player& opponent, Board& board);
};
