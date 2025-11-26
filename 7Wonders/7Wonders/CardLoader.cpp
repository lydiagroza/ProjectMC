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
        
        if (line.empty() || line.find_first_not_of(" \t\n\v\f\r,") == string::npos) {
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

        // atașează efecte în funcție de text
        auto effects = parseEffects(effectsStr);
        for (auto& ef : effects)
            card->addEffect(ef);

        cards.push_back(card);
    }

    return cards;
}

Color CardLoader::parseColor(const std::string& s) {
    // Creează o copie a string-ului și convertește-o la litere mici
    std::string lower_s = s;
    std::transform(lower_s.begin(), lower_s.end(), lower_s.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    static std::unordered_map<std::string, Color> map = {
        {"brown", Color::Brown}, {"gray", Color::Gray}, {"yellow", Color::Yellow},
        {"red", Color::Red}, {"blue", Color::Blue}, {"green", Color::Green}, {"purple", Color::Purple}
    };
    auto it = map.find(lower_s);

    if (it != map.end()) {
        return it->second;
    }
    
    // Dacă string-ul este gol sau nu este găsit, returnează o valoare default
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
    if (s.empty() || s == "free") {
        return cost;
    }

    std::stringstream ss(s);
    std::string item;

    // Permite costuri multiple separate prin ';' (ex: "1wood;1clay")
    while (std::getline(ss, item, ';')) {
        if (item.length() < 2) continue;

        // Extrage cantitatea și tipul resursei
        int quantity = item[0] - '0';
        std::string resource_str = item.substr(1);

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
    vector<std::function<void(Player&)>> effs;
    if (s.empty()) return effs;

    stringstream ss(s);
    string token;
    while (getline(ss, token, ';')) {
        token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
        if (token == "add_resource_wood") effs.push_back([](Player& p) {p.add_Resource(Resource::Wood,1); });
        else if (token == "add_resource_stone") effs.push_back([](Player& p) { p.add_Resource(Resource::Stone,1); });
        else if (token == "add_resource_clay") effs.push_back([](Player& p) { p.add_Resource(Resource::Clay,1); });
        else if (token == "add_resource_glass") effs.push_back([](Player& p) { p.add_Resource(Resource::Glass,1); });
        else if (token == "add_resource_papyrus") effs.push_back([](Player& p) { p.add_Resource(Resource::Papyrus,1); });
        else if (token == "add_VictoryPoint1") effs.push_back([](Player& p) { p.add_Points(Points::Victory,1);});
        else if (token == "add_VictoryPoint3") effs.push_back([](Player& p) { p.add_Points(Points::Victory,3);});
        else if (token == "add_MilitaryPoint1") effs.push_back([](Player& p) { p.add_Points(Points::Military,1); });
        else if (token == "add_coin4") effs.push_back([](Player& p) { p.addCoins(4); });
    }
    return effs;
}
