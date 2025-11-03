#include<string>
#include<unordered_map>
#include<unordered_set>
#include "CardBase.h"
class Player {
private:
	std::string m_name;
	std::uint8_t m_id; 
	std::uint16_t *nrCoins;
	std::unordered_map<Points, int> m_pointsScore;
	std::map<Resource, int>m_inventory ; // toate resursele jucatorului 
	std::unordered_set<CardBase> m_buildings; // cartile jucatorului
	std::vector<Wonder>availableWonders;
	std::vector<Wonder>builtWonders;
	std::unordered_set<Scientific_Symbol>m_scientificSymbols;
	std::unordered_set<Symbol>m_chainSymbols;
	
public:
	Player(const std::string& playerName);
/*	void addCoins(int amount);
	bool decreaseCoins(int amount*/);
	int calculateTradeCost(const CardBase&  c,  const Player& opponent)const;//calculeaza monezile necesare pentru a cumpara resursele necesare pentru a folosi o carte 
	bool canAffordConstruction (const CardBase & c, const Player& opponent);
	//void moveMilitaryScore(int shields,Player& opponent);
	//void addCards(const Card& card, Players& opponent, GameState& board);



	// fct pentru carti : 
	void add_Resource(Resource r, int amount);
	void add_Points(Points p, int amount);
	void add_ScientificSymbol(Scientific_Symbol symbol);
	void add_ChainSymbol(Symbol symbol);
};
