#include "GuildLoader.h"
#include "Player.h"
#include "Board.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <unordered_map>

using namespace std;

std::vector<std::shared_ptr<CardBase>> GuildLoader::loadFromCSV(const std::string& filename) {
    vector<shared_ptr<CardBase>> guilds;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Eroare: nu s-a putut deschide fisierul " << filename << endl;
        return guilds;
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        if (line.empty() || line.find_first_not_of(" \t\n\v\f\r,") == string::npos) {
            continue;
        }

        stringstream ss(line);
        string name, idStr, costStr, symbolStr, unlocksStr, effectsStr;

        getline(ss, name, ',');
        getline(ss, idStr, ',');
        getline(ss, costStr, ',');
        getline(ss, symbolStr, ',');
        getline(ss, unlocksStr, ',');
        getline(ss, effectsStr, '\n');

        // Trim function (same as CardLoader)
        auto trim = [](string s) {
            s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !isspace(ch);
                }));
            s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !isspace(ch);
                }).base(), s.end());
            s.erase(remove(s.begin(), s.end(), '"'), s.end());
            return s;
            };

        name = trim(name);
        idStr = trim(idStr);
        costStr = trim(costStr);
        effectsStr = trim(effectsStr);

        uint16_t id = static_cast<uint16_t>(stoi(idStr));
        map<Resource, uint8_t> cost = parseCost(costStr);

        // Guild-urile sunt ÎNTOTDEAUNA Purple ?i NU au symbol/unlocks
        auto guild = make_shared<CardBase>(
            name,
            id,
            Color::Purple,  // ? Toate guild-urile sunt Purple
            cost,
            std::nullopt,   // Nu au symbol
            std::nullopt    // Nu au unlocks
        );

        // Parse ?i adaug? efectele (doar efectele instant - coins)
        auto effects = parseGuildEffects(effectsStr);
        for (auto& ef : effects) {
            guild->addEffect(ef);
        }

        guilds.push_back(guild);
    }

    cout << "Loaded " << guilds.size() << " guilds from " << filename << endl;
    return guilds;
}

std::map<Resource, uint8_t> GuildLoader::parseCost(const std::string& s) {
    map<Resource, uint8_t> cost;
    if (s.empty()) return cost;

    stringstream ss(s);
    string item;

    while (getline(ss, item, ';')) {
        // Remove quotes and spaces
        item.erase(remove(item.begin(), item.end(), '"'), item.end());
        item.erase(remove_if(item.begin(), item.end(), ::isspace), item.end());

        if (item.empty()) continue;

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

std::vector<std::function<void(Player&, Board&, Player&)>> GuildLoader::parseGuildEffects(const std::string& s) {
    vector<function<void(Player&, Board&, Player&)>> effects;
    if (s.empty()) return effects;

    stringstream ss(s);
    string token;

    // ? Map pentru efectele INSTANT ale guild-urilor (doar coins)
    static const unordered_map<string, function<void(Player&, Board&, Player&)>> effectMap = {

        {"coins_per_green", [](Player& p, Board& b, Player& o) {
            auto countGreen = [](const Player& player) {
                return player.getInventory().count(Color::Green)
                    ? player.getInventory().at(Color::Green).size() : 0;
            };
            int maxGreen = std::max(countGreen(p), countGreen(o));
            p.addResource(Coin, maxGreen);
            cout << p.getName() << " gained " << maxGreen
                 << " coins from Scientists Guild." << endl;
        }},

        {"coins_per_brown_grey", [](Player& p, Board& b, Player& o) {
            auto countBrownGrey = [](const Player& player) {
                int count = 0;
                if (player.getInventory().count(Color::Brown))
                    count += player.getInventory().at(Color::Brown).size();
                if (player.getInventory().count(Color::Gray))
                    count += player.getInventory().at(Color::Gray).size();
                return count;
            };
            int maxCount = std::max(countBrownGrey(p), countBrownGrey(o));
            p.addResource(Coin, maxCount);
            cout << p.getName() << " gained " << maxCount
                 << " coins from Shipowners Guild." << endl;
        }},

        {"coins_per_yellow", [](Player& p, Board& b, Player& o) {
            auto countYellow = [](const Player& player) {
                return player.getInventory().count(Color::Yellow)
                    ? player.getInventory().at(Color::Yellow).size() : 0;
            };
            int maxYellow = std::max(countYellow(p), countYellow(o));
            p.addResource(Coin, maxYellow);
            cout << p.getName() << " gained " << maxYellow
                 << " coins from Traders Guild." << endl;
        }},

        {"coins_per_blue", [](Player& p, Board& b, Player& o) {
            auto countBlue = [](const Player& player) {
                return player.getInventory().count(Color::Blue)
                    ? player.getInventory().at(Color::Blue).size() : 0;
            };
            int maxBlue = std::max(countBlue(p), countBlue(o));
            p.addResource(Coin, maxBlue);
            cout << p.getName() << " gained " << maxBlue
                 << " coins from Magistrates Guild." << endl;
        }},

        {"coins_per_red", [](Player& p, Board& b, Player& o) {
            auto countRed = [](const Player& player) {
                return player.getInventory().count(Color::Red)
                    ? player.getInventory().at(Color::Red).size() : 0;
            };
            int maxRed = std::max(countRed(p), countRed(o));
            p.addResource(Coin, maxRed);
            cout << p.getName() << " gained " << maxRed
                 << " coins from Tacticians Guild." << endl;
        }}

        // ? Builders ?i Moneylenders NU au efecte instant
        // VP-urile lor se calculeaz? în Player::calculateGuildVictoryPoints()
    };

    while (getline(ss, token, ';')) {
        // Clean token
        token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
        token.erase(remove(token.begin(), token.end(), '"'), token.end());

        if (token.empty()) continue;

        auto it = effectMap.find(token);
        if (it != effectMap.end()) {
            effects.push_back(it->second);
        }
        else {
            // Dac? nu g?se?te efectul, nu e eroare - poate fi guild f?r? efect instant
            // (Builders ?i Moneylenders)
            if (!token.empty()) {
                cout << "Guild effect '" << token << "' has no instant effect (VP calculated at game end)." << endl;
            }
        }
    }

    return effects;
}