#pragma once
#include "CardLoader.h"


    static std::vector<std::shared_ptr<CardBase>> loadFromCSV(const std::string& filename) {
        using namespace std;
        vector<shared_ptr<CardBase>> cards;
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Eroare: nu s-a putut deschide fisierul " << filename << endl;
            return cards;
        }

        string line;
        getline(file, line); // skip header

        while (getline(file, line)) { // impartim pe coloane 
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
            auto trim = [](string s) {
                s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
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

    static Color parseColor(const std::string& s) {
        static std::unordered_map<std::string, Color> map = {
            {"Brown", Color::Brown}, {"Gray", Color::Gray}, {"Yellow", Color::Yellow},
            {"Red", Color::Red}, {"Blue", Color::Blue}, {"Green", Color::Green}, {"Purple", Color::Purple}
        };
        auto it = map.find(s);
        return it != map.end() ? it->second : Color::Brown;
    }

    static std::optional<Symbol> parseSymbol(const std::string& s) {
        if (s.empty()) return std::nullopt;
        static std::unordered_map<std::string, Symbol> map = {
            {"book", Symbol::Book}, {"mask", Symbol::Mask}, {"horse", Symbol::Horse},{"lyre", Symbol::Lyre},
            {"gear", Symbol::Gear},
            {"barrel", Symbol::Barrel}, 
			{"helmet", Symbol::Helmet}, {"sun", Symbol::Sun},{"target", Symbol::Target}, {"pot", Symbol:: Pot},
            {"column", Symbol:: Column}, {"temple", Symbol :: Temple}
            {"castle", Symbol::Castle}, {"droplet", Symbol::Droplet}, {"moon", Symbol::Moon},
            {"vase", Symbol::Vase}, {"sword", Symbol::Sword}
        };
        auto it = map.find(s);
        if (it != map.end()) return it->second;
        return std::nullopt;
    }

    static std::optional<std::vector<Symbol>> parseUnlocks(const std::string& s) {
        if (s.empty()) return std::nullopt;
        std::vector<Symbol> unlocks;
        // aici ai putea face split dacă vrei mai multe unlocks
        return unlocks;
    }

    static std::map<Resource, uint8_t> parseCost(const std::string& s) {
        std::map<Resource, uint8_t> cost;
        if (s.empty()) return cost;

        if (s.find("coin") != std::string::npos) cost[Resource::Coin] = s.back() - '0';
        else if (s.find("wood") != std::string::npos) cost[Resource::Wood] = s.back() - '0';
        else if (s.find("stone") != std::string::npos) cost[Resource::Stone] = s.back() - '0';
        else if (s.find("clay") != std::string::npos) cost[Resource::Clay] = s.back() - '0';
        else if (s.find("glass") != std::string::npos) cost[Resource::Glass] = s.back() - '0';
        else if (s.find("papyrus") != std::string::npos) cost[Resource::Papyrus] = s.back() - '0';
        return cost;
    }

    static std::vector<std::function<void(Player&)>> parseEffects(const std::string& s) {
        using namespace std;
        vector<std::function<void(Player&)>> effs;
        if (s.empty()) return effs;

        stringstream ss(s);
        string token;
        while (getline(ss, token, ';')) {
            token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
            if (token == "add_resource_wood") effs.push_back([](Player& p) { /*p.addResource(Resource::Wood,1);*/ });
            else if (token == "add_resource_stone") effs.push_back([](Player& p) { /*p.addResource(Resource::Stone,1);*/ });
            else if (token == "add_resource_clay") effs.push_back([](Player& p) { /*p.addResource(Resource::Clay,1);*/ });
            else if (token == "add_resource_glass") effs.push_back([](Player& p) { /*p.addResource(Resource::Glass,1);*/ });
            else if (token == "add_resource_papyrus") effs.push_back([](Player& p) { /*p.addResource(Resource::Papyrus,1);*/ });
            else if (token == "add_VictoryPoint1") effs.push_back([](Player& p) { /*p.addPoints(Points::Victory,1);*/ });
            else if (token == "add_VictoryPoint3") effs.push_back([](Player& p) { /*p.addPoints(Points::Victory,3);*/ });
            else if (token == "add_MilitaryPoint1") effs.push_back([](Player& p) { /*p.addPoints(Points::Military,1);*/ });
            else if (token == "add_coin4") effs.push_back([](Player& p) { /*p.addCoins(4);*/ });
        }
        return effs;
    }
