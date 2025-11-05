#include<string>
#include<unordered_map>
#include<unordered_set>
#include "CardBase.h"

class Player {
private:
	std::string name;
	int nrCoins;
	int militaryScore;
	std::unordered_map<Resource, int> baseProduction;
	std::unordered_map<Points, int>m_pointsScore;
	std::unordered_map<Resource, int>fixedCost; // stocheaza resursele primare care pot fi cumparate cu discount
	std::unordered_map<Resource, int>bonusProduction;//stocheaza resursele obtinute din cartile galbene sau minuni care nu trebuie sa afecteze costul de cumparare
	std::unordered_map<Color, std::vector<CardBase>>cityBuildings;
	std::vector<Wonder>availableWonders;
	std::vector<Wonder>builtWonders;
	std::unordered_set<Scientific_Symbol>scientificSymbols;
	std::unordered_set<Symbol>chainSymbols;
public:
	Player(const std::string& playerName);
	bool decreaseCoins(int amount);
	bool hasSufficientResources(const CardBase& c)const;
	int calculateTradeCost(const CardBase& c, const Player& opponent)const;//calculeaza monezile necesare pentru a cumpara resursele necesare pentru a folosi o carte 
	bool canAffordConstruction(const CardBase& c, const Player& opponent);
	void add_Resource(Resource r, int amount);
	void add_Points(Points p, int amount);
	void add_ScientificSymbol(Scientific_Symbol symbol);
	void add_ChainSymbol(Symbol symbol);
};
