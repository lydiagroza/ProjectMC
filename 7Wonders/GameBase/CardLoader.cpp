#include "CardBase.h"
#include "Player.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <memory>
#include<optional>
#include <unordered_map>
#include <cctype>
#include "Cardloader.h"

using namespace std;

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
        if (line.empty() || line.find_first_not_of(" \t\n\v\f\r,") == string::npos) continue;

        stringstream ss(line);
        string name, idStr, colorStr, costStr, symbolStr, unlocksStr, effectsStr, destroyStr;

        getline(ss, name, ',');
        getline(ss, idStr, ',');
        getline(ss, colorStr, ',');
        getline(ss, costStr, ',');
        getline(ss, symbolStr, ',');
        getline(ss, unlocksStr, ',');
        getline(ss, effectsStr, ',');
        getline(ss, destroyStr, '\n');

        auto trim = [](string s) {
            s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) { return !isspace(ch); }));
            s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !isspace(ch); }).base(), s.end());
            s.erase(remove(s.begin(), s.end(), '"'), s.end());
            return s;
            };

        name = trim(name);
        idStr = trim(idStr);
        colorStr = trim(colorStr);
        costStr = trim(costStr);
        symbolStr = trim(symbolStr);
        unlocksStr = trim(unlocksStr);
        effectsStr = trim(effectsStr);
        destroyStr = trim(destroyStr);

        uint16_t id = static_cast<uint16_t>(stoi(idStr));
        Color color = parseColor(colorStr);
        map<Resource, uint8_t> cost = parseCost(costStr);
        auto symbol = parseSymbol(symbolStr);
        auto unlocks = parseUnlocks(unlocksStr);

        auto card = make_shared<CardBase>(name, id, color, cost, symbol, unlocks);

        auto effects = parseEffects(effectsStr);
        for (auto& ef : effects) card->addEffect(ef);

        card->m_destroy = parseDestroy(destroyStr);

        cards.push_back(card);
    }

    return cards;
}

Color CardLoader::parseColor(const string& s) {
    static unordered_map<string, Color> map = {
        {"Brown", Color::Brown}, {"Gray", Color::Gray}, {"Yellow", Color::Yellow},
        {"Red", Color::Red}, {"Blue", Color::Blue}, {"Green", Color::Green}, {"Purple", Color::Purple}
    };
    auto it = map.find(s);
    return (it != map.end()) ? it->second : Color::Brown;
}

std::optional<Symbol> CardLoader::parseSymbol(const string& s) {
    if (s.empty()) return std::nullopt;
    static unordered_map<string, Symbol> map = {
        {"book", Symbol::Book}, {"mask", Symbol::Mask}, {"horse", Symbol::Horse},{"lyre", Symbol::Lyre},
        {"gear", Symbol::Gear},{"barrel", Symbol::Barrel},{"helmet", Symbol::Helmet},
        {"sun", Symbol::Sun},{"target", Symbol::Target},{"pot", Symbol::Pot},{"column", Symbol::Column},
        {"temple", Symbol::Temple},{"castle", Symbol::Castle},{"droplet", Symbol::Droplet},
        {"moon", Symbol::Moon},{"vase", Symbol::Vase},{"sword", Symbol::Sword}
    };
    auto it = map.find(s);
    if (it == map.end()) return nullopt; 
}

std::optional<Symbol> CardLoader::parseUnlocks(const string& s) {
    return parseSymbol(s); // exact aceeasi logica
}

std::map<Resource, uint8_t> CardLoader::parseCost(const string& s) {
    map<Resource, uint8_t> cost;
    if (s.empty()) return cost;

    stringstream ss(s);
    string item;
    while (getline(ss, item, ';')) {
        int quantity = item[0] - '0';
        string resource_str = item.substr(1);
        if (resource_str == "wood") cost[Resource::Wood] = quantity;
        else if (resource_str == "clay") cost[Resource::Clay] = quantity;
        else if (resource_str == "stone") cost[Resource::Stone] = quantity;
        else if (resource_str == "glass") cost[Resource::Glass] = quantity;
        else if (resource_str == "papyrus") cost[Resource::Papyrus] = quantity;
        else if (resource_str == "coin") cost[Resource::Coin] = quantity;
    }
    return cost;
}

std::vector<std::function<void(Player&,Board&, Player&)>> CardLoader::parseEffects(const std::string& s) {
    using namespace std;
    vector<std::function<void(Player&, Board&, Player&)>> effects;
    if (s.empty()) return effects;

    stringstream ss(s);
    string token;

    // map for fixed (no-parameter) effects - păstrat exact cum l-ai avut
    static const unordered_map<string, function<void(Player&,Board&, Player&)>> effectMap = {
        {"add_resource_wood", [](Player& p,Board& b, Player&) { p.addResource(Resource::Wood, 1); }},
        {"add_resource_coin4", [](Player& p,Board& b, Player&) { p.addResource(Resource::Coin, 4); }},
        {"add_resource_coin6", [](Player& p,Board& b, Player&) { p.addResource(Resource::Coin, 6); }},
        {"add_resource_clay2", [](Player& p,Board& b, Player&) { p.addResource(Resource::Clay, 2); }},
        {"add_resource_wood2", [](Player& p,Board& b, Player&) { p.addResource(Resource::Wood, 2); }},
        {"add_resource_stone2", [](Player& p,Board& b, Player&) { p.addResource(Resource::Stone, 2); }},
        {"add_resource_stone", [](Player& p,Board& b, Player&) { p.addResource(Resource::Stone, 1); }},
        {"add_resource_clay", [](Player& p,Board& b, Player&) { p.addResource(Resource::Clay, 1); }},
        {"add_resource_glass", [](Player& p,Board& b, Player&) { p.addResource(Resource::Glass, 1); }},
        {"add_resource_papyrus", [](Player& p,Board& b, Player&) { p.addResource(Resource::Papyrus, 1); }},
        {"add_scientific_symbol_ink", [](Player& p,Board& b,Player&) { p.add_ScientificSymbol(Scientific_Symbol::Ink); }},
        {"add_scientific_symbol_scales", [](Player& p,Board& b, Player&) { p.add_ScientificSymbol(Scientific_Symbol::Scales); }},
        {"add_scientific_symbol_mortar", [](Player& p,Board& b, Player&) { p.add_ScientificSymbol(Scientific_Symbol::Ink); }},
        {"add_scientific_symbol_gyroscope", [](Player& p,Board& b, Player&) { p.add_ScientificSymbol(Scientific_Symbol::Gyroscope); }},
        {"add_scientific_symbol_sun_dial", [](Player& p,Board& b, Player&) { p.add_ScientificSymbol(Scientific_Symbol::Sun_Dial); }},
        {"add_scientific_symbol_wheel", [](Player& p,Board &b, Player&) { p.add_ScientificSymbol(Scientific_Symbol::Wheel); }},
        {"coin2Wonder",[](Player& p,Board& b, Player&) { p.addResource(Resource::Coin, 2 * p.getWonders().size()); }},
        {"coin2Brown",[](Player& p,Board& b, Player&) { p.addResource(Resource::Coin, 2 * p.getInventory().at(Color::Brown).size()); }},
        {"coin1Yellow",[](Player& p,Board& b, Player&) { p.addResource(Resource::Coin, p.getInventory().at(Color::Yellow).size()); }},
        {"coin3Gray",[](Player& p,Board& b, Player&) { p.addResource(Resource::Coin, 3 * p.getInventory().at(Color::Gray).size()); }},
        {"coin1Red",[](Player& p,Board& b, Player&) { p.addResource(Resource::Coin, p.getInventory().at(Color::Red).size()); }},
        {"sale_stone1",[](Player& p,Board& b, Player&) { p.set_discountedResource(Stone); }},
        {"sale_wood1",[](Player& p,Board& b, Player&) { p.set_discountedResource(Wood); }},
        {"sale_clay1",[](Player& p,Board& b, Player&) { p.set_discountedResource(Clay); }},
        {"add_resource_glass/papyrus",[](Player& p,Board& b, Player&) {
			std::vector<Resource> choices = { Resource::Glass, Resource::Papyrus };
            p.addChoiceResources(choices);
        }},
        {"add_resource_wood/clay/stone",[](Player& p,Board& b, Player&) {
        std:: vector<Resource> choices ={ Resource::Wood, Resource::Clay, Resource::Stone };
		p.addChoiceResources(choices);
        }}
    };

    const string vpPrefix = "add_VictoryPoint";
    const string mpPrefix = "add_MilitaryPoint";

    while (getline(ss, token, ';')) {
        int ok = 0;
        token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());

        if (token.rfind(vpPrefix, 0) == 0) {
            int amount = stoi(token.substr(vpPrefix.size()));
            effects.push_back([amount](Player& p, Board &b, Player&) { 
                p.add_Points(Points::Victory, static_cast<std::uint8_t>(amount)); });
            ok = 1;
            continue;
        }
        else if (token.rfind(mpPrefix, 0) == 0) {
            // Extragem numărul de scuturi (ex: "add_MilitaryPoint2" -> 2)
            int amount = stoi(token.substr(mpPrefix.size()));

            // Lambda-ul primește Player (self), Board (b) și Player (opponent)
            effects.push_back([amount](Player& p, Board& b, Player& opponent) {
                int coinsToTake = 0;

                // 1. Apelăm logica militară care mută pionul
                // p.getId() ar trebui să returneze 1 sau 2
                b.getMilitaryTrack().applyShields(p.getId(), amount, coinsToTake);

                // 2. Dacă s-a trecut peste un jeton, coinsToTake va fi > 0
                if (coinsToTake > 0) {
                    std::cout << "[MILITAR] " << opponent.getName() << " pierde " << coinsToTake << " banuti!\n";
                    opponent.removeResource(Resource::Coin, coinsToTake);
                }
                });
            ok = 1;
            continue;
        }

        auto it = effectMap.find(token);
        if (it != effectMap.end()) {
            effects.push_back(it->second);
            ok = 1;
            continue;
        }
        if (ok==0)
        cout << "Effect " << token << " unknown" << endl;
    }

    return effects;
}


std::optional<std::function<void(Player&,Board &, Player&)>> CardLoader::parseDestroy(const string& s) {
    if (s.empty()) return std::nullopt;
    static const unordered_map<string, function<void(Player&, Board &, Player&)>> destroyMap = {
        {"!add_resource_wood", [](Player& p, Board&, Player&) {p.removeResource(Resource::Wood,1); }},
        {"!add_resource_clay2", [](Player& p,Board&, Player&) {p.removeResource(Resource::Clay,2); }},
        {"!add_resource_wood2", [](Player& p,Board &, Player&) {p.removeResource(Resource::Wood,2); }},
        {"!add_resource_stone2", [](Player& p,Board &, Player&) {p.removeResource(Resource::Stone,2); }},
        {"!add_resource_stone", [](Player& p, Board &,Player&) {p.removeResource(Resource::Stone,1); }},
        {"!add_resource_clay", [](Player& p, Board &, Player&) {p.removeResource(Resource::Clay,1); }},
        {"!add_resource_glass", [](Player& p, Board &, Player&) {p.removeResource(Resource::Glass,1); }},
        {"!add_resource_papyrus", [](Player& p, Board &, Player&) {p.removeResource(Resource::Papyrus,1); }}
    };
    auto it = destroyMap.find(s);
    if (it != destroyMap.end()) return it->second;
    std::cerr << "Unknown destroy effect: " << s << std::endl;
    return std::nullopt;
}













