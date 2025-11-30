#include "CardLoader.h"
#include "Player.h"
#include <algorithm> // Pentru std::transform
#include <cctype>    // Pentru ::tolower

std::vector<std::shared_ptr<CardBase>> CardLoader::loadFromCSV(const std::string& filename) {
    using namespace std;
    vector<shared_ptr<CardBase>> cards;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Eroare: nu s-a putut deschide fisierul " << filename << endl;
        return cards;
    }

    string line;
    getline(file, line); // skip header

    while (getline(file, line)) { 
        
        if (line.empty() == string::npos) {
            continue;
        }// impartim pe coloane 
        stringstream ss(line);
        string name, idStr, colorStr, costStr, symbolStr, unlocksStr, effectsStr;

        getline(ss, name, ',');
        getline(ss, idStr, ',');
        getline(ss, colorStr, ',');
        getline(ss, costStr, ',');
        getline(ss, symbolStr, ',');
        getline(ss, unlocksStr, ',');
        getline(ss, effectsStr, '\n');

        // scoatem "" si spatiile 
        auto trim = [](std::string s) {
            // elimină spațiile de la început
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
                }));
            // elimină spațiile de la sfârșit
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
                }).base(), s.end());
            // elimină ghilimelele
            s.erase(std::remove(s.begin(), s.end(), '"'), s.end());
            return s;
            };


        name = trim(name);
        idStr = trim(idStr);
        colorStr = trim(colorStr);
        costStr = trim(costStr);
        symbolStr = trim(symbolStr);
        unlocksStr = trim(unlocksStr);
        effectsStr = trim(effectsStr);

        // conversii
        uint16_t id = static_cast<uint16_t>(stoi(idStr));
        Color color = parseColor(colorStr);
        map<Resource, uint8_t> cost = parseCost(costStr);
        auto symbol = parseSymbol(symbolStr);
        auto unlocks = parseUnlocks(unlocksStr);

        auto card = make_shared<CardBase>(name, id, color, cost, symbol, unlocks);

        //adauga efectele
        auto effects = parseEffects(effectsStr);
        for (auto& ef : effects)
            card->addEffect(ef);

        cards.push_back(card);
    }

    return cards;
}

Color CardLoader::parseColor(const std::string& s) {
    
    static std::unordered_map<std::string, Color> map = {
        {"Brown", Color::Brown}, {"Gray", Color::Gray}, {"Yellow", Color::Yellow},
        {"Red", Color::Red}, {"Blue", Color::Blue}, {"Green", Color::Green}, {"Purple", Color::Purple}
    };
    auto it = map.find(s);

    if (it != map.end()) {
        return it->second;
    }
    
    
    return Color::Brown; 
}

std::optional<Symbol> CardLoader::parseSymbol(const std::string& s) {
    if (s.empty()) return std::nullopt;
    static std::unordered_map<std::string, Symbol> map = {
        {"book", Symbol::Book}, {"mask", Symbol::Mask}, {"horse", Symbol::Horse},{"lyre", Symbol::Lyre},
        {"gear", Symbol::Gear},
        {"barrel", Symbol::Barrel}, 
        {"helmet", Symbol::Helmet}, {"sun", Symbol::Sun},{"target", Symbol::Target}, {"pot", Symbol:: Pot},
        {"column", Symbol:: Column}, {"temple", Symbol :: Temple},
        {"castle", Symbol::Castle}, {"droplet", Symbol::Droplet}, {"moon", Symbol::Moon},
        {"vase", Symbol::Vase}, {"sword", Symbol::Sword}
    };
    auto it = map.find(s);
    if (it != map.end()) return it->second;
    return std::nullopt;
}

std::optional<Symbol> CardLoader::parseUnlocks(const std::string & s) {
    if (s.empty()) return std::nullopt;
        static std::unordered_map<std::string, Symbol> map = {
               {"book", Symbol::Book}, {"mask", Symbol::Mask}, {"horse", Symbol::Horse},{"lyre", Symbol::Lyre},
               {"gear", Symbol::Gear},
               {"barrel", Symbol::Barrel},
               {"helmet", Symbol::Helmet}, {"sun", Symbol::Sun},{"target", Symbol::Target}, {"pot", Symbol::Pot},
               {"column", Symbol::Column}, {"temple", Symbol::Temple},
               {"castle", Symbol::Castle}, {"droplet", Symbol::Droplet}, {"moon", Symbol::Moon},
               {"vase", Symbol::Vase}, {"sword", Symbol::Sword}
    };
        auto it = map.find(s); 
        if (it != map.end()) return it->second; 
    return std::nullopt;
}

std::map<Resource, uint8_t> CardLoader::parseCost(const std::string& s) {
    std::map<Resource, uint8_t> cost;
    if (s.empty()) {
        return cost;
    }

    std::stringstream ss(s);
    std::string item;

    // Permite costuri multiple separate prin ';' 
    while (std::getline(ss, item, ';')) {
       // Extrage cantitatea și tipul resursei
        int quantity = item[0] - '0';
        std::string resource_str = item.substr(1); // string nou de la 1 pana la final 

        if (resource_str == "wood") cost[Resource::Wood] = quantity;
        else if (resource_str == "clay") cost[Resource::Clay] = quantity;
        else if (resource_str == "stone") cost[Resource::Stone] = quantity;
        else if (resource_str == "glass") cost[Resource::Glass] = quantity;
        else if (resource_str == "papyrus") cost[Resource::Papyrus] = quantity;
        else if (resource_str == "coin") cost[Resource::Coin] = quantity;
    }
    return cost;
}

std::vector<std::function<void(Player&)>> CardLoader::parseEffects(const std::string& s) {
    using namespace std;
    vector<std::function<void(Player&)>> effects;
    if (s.empty()) return effects;

    stringstream ss(s);
    string token;

    // map for fixed (no-parameter) effects
    static const unordered_map<string, function<void(Player&)>> effectMap = {
        {"add_resource_wood", [](Player& p) { p.add_Resource(Resource::Wood, 1); }},
        {"add_resource_stone", [](Player& p) { p.add_Resource(Resource::Stone, 1); }},
        {"add_resource_clay", [](Player& p) { p.add_Resource(Resource::Clay, 1); }},
        {"add_resource_glass", [](Player& p) { p.add_Resource(Resource::Glass, 1); }},
        {"add_resource_papyrus", [](Player& p) { p.add_Resource(Resource::Papyrus, 1); }},
        {"coin2Wonder",[](Player& p) {
            for (int i=0; i<p.getWonders().size(); i++)
				p.add_Resource(Resource::Coin, 2);
        
}},
        {"coin2Brown",[](Player& p) {
        for (int i=0;i< p.getInventory().at(Color::Brown).size(); i++)
			p.add_Resource(Resource::Coin, 2);
        }},
        {"coin1Yellow",[](Player& p) {
		for (int i = 0; i < p.getInventory().at(Color::Yellow).size(); i++)
			p.add_Resource(Resource::Coin, 1);
        }},
        {"coin3Gray",[](Player& p) {
		for (int i = 0; i < p.getInventory().at(Color::Gray).size(); i++)
			p.add_Resource(Resource::Coin, 3);
        }},
        {"coin1Red",[](Player& p) {
		for(int i = 0; i < p.getInventory().at(Color::Red).size(); i++)
			p.add_Resource(Resource::Coin, 1);
        }}
   
    };

    const string vpPrefix = "add_VictoryPoint";
    const string mpPrefix = "add_MilitaryPoint";

    while (getline(ss, token, ';')) {
        token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());

        // numeric-suffixed tokens (variable amount)
        if (token.starts_with(vpPrefix)) {
            int amount = stoi(token.substr(vpPrefix.size()));
            effects.push_back([amount](Player& p) { p.add_Points(Points::Victory, static_cast<std::uint8_t>(amount)); });
            continue;
        }
        if (token.starts_with(mpPrefix)) {
            int amount = stoi(token.substr(mpPrefix.size()));
            effects.push_back([amount](Player& p) { p.add_Points(Points::Military, static_cast<std::uint8_t>(amount)); });
            continue;
        }

 // fixed tokens 
        auto it = effectMap.find(token);
        if (it != effectMap.end()) {
            effects.push_back(it->second);
            continue;
        }
;
    }

    return effects;
}
