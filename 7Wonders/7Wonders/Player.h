#include<string>
#include<unordered_map>
#include<map>
#include<vector>
#include<unordered_set>
#include "CardBase.h"
#include<cstdint>

class Player {
private:
	std::string m_name;
	std::uint8_t m_nrCoins;
	std::uint8_t m_militaryScore;
	std::unordered_map<Resource, std::uint8_t> m_baseResources;
	std::unordered_map<Points, std::uint8_t>m_pointsScore;
	std::unordered_map<Resource, std::uint8_t>m_discountedResource; // stocheaza resursele primare care pot fi cumparate cu discount
	std::unordered_map<Resource, std::uint8_t>m_chosenResource;//stocheaza resursele obtinute din cartile galbene sau minuni care nu trebuie sa afecteze costul de cumparare
	std::map<Color, std::vector<CardBase>>m_Inventory;
	std::vector<Wonder>m_availableWonders;
	std::vector<Wonder>m_builtWonders;
	std::unordered_set<Scientific_Symbol>m_scientificSymbols;
	std::unordered_set<Symbol>m_chainSymbols;
public:
	Player(const std::string& playerName);
	bool decreaseCoins(std::uint8_t amount);
	void addCoins(std::uint8_t amount);
	bool hasSufficientResources(const CardBase& c)const;
	int calculateTradeCost(const CardBase& c, const Player& opponent)const;//calculeaza monezile necesare pentru a cumpara resursele necesare pentru a folosi o carte 
	bool canAffordConstruction(const CardBase& c, const Player& opponent);
	void add_Resource(Resource r, int amount);
	void add_Points(Points p, int amount);
	void add_ScientificSymbol(Scientific_Symbol symbol);
	void add_ChainSymbol(Symbol symbol);
	void add_Coins();
	void add_Coin3_per_trade_good_sold_to_neighbors();
	void add_DiscountedResource(Resource r, int discount);
	
	
};
