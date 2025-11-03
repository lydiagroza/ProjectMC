#include<string>
#include<unordered_map>
#include<unordered_set>
class Player {
private:
	std::string name;
	int nrCoins;
	int militaryScore;
	std::unordered_map<std::string, int> rawMaterials;
	std::unordered_map<std::string, int>manufacturedGoods;
	std::unordered_map<std::string, int>fixedCost; // stocheaza resursele primare care pot fi cumparate cu discount
	std::unordered_map<std::string, int>bonusProduction;//stocheaza resursele obtinute din cartile galbene sau minuni
	std::unordered_map<std::string, std::vector<Card>>cityBuildings;
	std::vector<Wonder>availableWonders;
	std::vector<Wonder>builtWonders;
	std::unordered_set<std::string>scientificSymbols;
	std::unordered_set<std::string>chainSymbols;
public:
	Player(const std::string& playerName);
	void addCoins(int amount);
	bool decreaseCoins(int amount);
	int calculateTradeCost(const std::unordered_map<std::string, int>& requiredResurces, const Player& opponent)const;//calculeaza monezile necesare pentru a cumpara resursele necesare pentru a folosi o carte 
	bool canAffordConstruction(int fixedCoinCost, const std::unordered_map<std::string, int>& requiredResources, const Player& opponent);
	//void moveMilitaryScore(int shields,Player& opponent);
	//void addCards(const Card& card, Players& opponent, GameState& board);

};
