#include "CardBase.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_map>

using namespace std;
class CardLoader {
public:
    static std::vector<std::shared_ptr<CardBase>> loadFromCSV(const std::string& filename);
    static Color parseColor(const std::string& s);
    static std::optional<Symbol> parseSymbol(const std::string& s);
	static std::optional<Symbol> parseUnlocks(const std::string& s);
    static std::map<Resource, uint8_t> parseCost(const std::string& s);
    static std::vector<std::function<void(Player&)>> parseEffects(const std::string& s); 
};

