#include "UniversalCardLoader.h"
#include "Game.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ranges>
#include <format>
#include <iostream>

using namespace std;

// --- UTILITARE ---
string UniversalCardLoader::trim(string s) {
    s.erase(remove(s.begin(), s.end(), '"'), s.end()); // Remove quotes
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    s.erase(s.find_last_not_of(" \t\r\n") + 1);
    return s;
}

map<Resource, uint8_t> UniversalCardLoader::parseCost(const string& s) {
    map<Resource, uint8_t> cost;
    if (s.empty()) return cost;
    stringstream ss(s);
    string item;
    while (getline(ss, item, ';')) {
        item = trim(item);
        if (item.empty()) continue;

        auto quantity = 1;
        string resStr;
        if (isdigit(item[0])) {
            quantity = item[0] - '0';
            resStr = item.substr(1);
        }
        else if (isdigit(item.back())) {
            quantity = item.back() - '0';
            resStr = item.substr(0, item.size() - 1);
        }
        else resStr = item;

        resStr = trim(resStr);
        if (resStr == "wood") cost[Resource::Wood] = quantity;
        else if (resStr == "clay") cost[Resource::Clay] = quantity;
        else if (resStr == "stone") cost[Resource::Stone] = quantity;
        else if (resStr == "glass") cost[Resource::Glass] = quantity;
        else if (resStr == "papyrus") cost[Resource::Papyrus] = quantity;
        else if (resStr.find("coin") != string::npos) cost[Resource::Coin] = quantity;
    }
    return cost;
}

Color UniversalCardLoader::parseColor(const string& s) {
    if (s == "Brown") return Color::Brown;
    if (s == "Gray") return Color::Gray;
    if (s == "Yellow") return Color::Yellow;
    if (s == "Red") return Color::Red;
    if (s == "Blue") return Color::Blue;
    if (s == "Green") return Color::Green;
    if (s == "Purple") return Color::Purple;
    return Color::Brown;
}

optional<Symbol> UniversalCardLoader::parseSymbol(const string& s) {
    if (s.empty()) return nullopt;
    string lower_s = s;
    std::ranges::transform(lower_s, lower_s.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    static const unordered_map<string, Symbol> symMap = {
        {"barrel", Barrel}, {"gear", Gear}, {"book", Book}, {"temple", Temple},
        {"target", Target}, {"lyre", Lyre}, {"castle", Castle}, {"droplet", Droplet},
        {"vase", Vase}, {"column", Column}, {"sword", Sword}, {"pot", Pot},
        {"horse", Horse}, {"helmet", Helmet}, {"mask", Mask}, {"sun", Sun}, {"moon", Moon}
    };
    auto it = symMap.find(lower_s);
    return (it != symMap.end()) ? make_optional(it->second) : nullopt;
}

// --- EFECTE ---
vector<function<void(Player&, Board&, Player&)>> UniversalCardLoader::parseEffects(const string& s) {
    vector<function<void(Player&, Board&, Player&)>> effects;
    if (s.empty()) return effects;

    stringstream ss(s);
    string token;

    static const std::unordered_map<std::string, std::function<void(Player&, Board&, Player&)>> effectMap = {
        //Effects for the 3 ages
         {"add_resource_wood", [](Player& p, Board&, Player&) { p.addResource(Resource::Wood, 1); }},
        {"add_resource_coin4", [](Player& p, Board&, Player&) { p.addResource(Resource::Coin, 4); }},
        {"add_resource_coin6", [](Player& p, Board&, Player&) { p.addResource(Resource::Coin, 6); }},
        {"add_resource_clay2", [](Player& p, Board&, Player&) { p.addResource(Resource::Clay, 2); }},
        {"add_resource_wood2", [](Player& p, Board&, Player&) { p.addResource(Resource::Wood, 2); }},
        {"add_resource_stone2", [](Player& p, Board&, Player&) { p.addResource(Resource::Stone, 2); }},
        {"add_resource_stone", [](Player& p, Board&, Player&) { p.addResource(Resource::Stone, 1); }},
        {"add_resource_clay", [](Player& p, Board&, Player&) { p.addResource(Resource::Clay, 1); }},
        {"add_resource_glass", [](Player& p, Board&, Player&) { p.addResource(Resource::Glass, 1); }},
        {"add_resource_papyrus", [](Player& p, Board&, Player&) { p.addResource(Resource::Papyrus, 1); }},
        {"add_scientific_symbol_ink", [](Player& p, Board&, Player&) { if (p.add_ScientificSymbol(Scientific_Symbol::Ink)) { Game::currentGame->handleProgressTokenChoice(); } }},
        {"add_scientific_symbol_scales", [](Player& p, Board&, Player&) { if (p.add_ScientificSymbol(Scientific_Symbol::Scales)) { Game::currentGame->handleProgressTokenChoice(); } }},
        {"add_scientific_symbol_mortar", [](Player& p, Board&, Player&) { if (p.add_ScientificSymbol(Scientific_Symbol::Mortar)) { Game::currentGame->handleProgressTokenChoice(); } }},
        {"add_scientific_symbol_gyroscope", [](Player& p, Board&, Player&) { if (p.add_ScientificSymbol(Scientific_Symbol::Gyroscope)) { Game::currentGame->handleProgressTokenChoice(); } }},
        {"add_scientific_symbol_sun_dial", [](Player& p, Board&, Player&) { if (p.add_ScientificSymbol(Scientific_Symbol::Sun_Dial)) { Game::currentGame->handleProgressTokenChoice(); } }},
        {"add_scientific_symbol_wheel", [](Player& p, Board&, Player&) { if (p.add_ScientificSymbol(Scientific_Symbol::Wheel)) { Game::currentGame->handleProgressTokenChoice(); } }},
        {"coin2Wonder", [](Player& p, Board&, Player&) { p.addResource(Resource::Coin, (uint8_t)(2 * p.getWonders().size())); }},
        {"coin2Brown", [](Player& p, Board&, Player&) { 
            size_t count = p.getInventory().count(Color::Brown) ? p.getInventory().at(Color::Brown).size() : 0;
            p.addResource(Resource::Coin, (uint8_t)(2 * count)); 
        }},
        {"coin1Yellow", [](Player& p, Board&, Player&) { 
            size_t count = p.getInventory().count(Color::Yellow) ? p.getInventory().at(Color::Yellow).size() : 0;
            p.addResource(Resource::Coin, (uint8_t)count); 
        }},
        {"coin3Gray", [](Player& p, Board&, Player&) { 
            size_t count = p.getInventory().count(Color::Gray) ? p.getInventory().at(Color::Gray).size() : 0;
            p.addResource(Resource::Coin, (uint8_t)(3 * count)); 
        }},
        {"coin1Red", [](Player& p, Board&, Player&) { 
            size_t count = p.getInventory().count(Color::Red) ? p.getInventory().at(Color::Red).size() : 0;
            p.addResource(Resource::Coin, (uint8_t)count); 
        }},
        {"sale_stone1", [](Player& p, Board&, Player&) { p.set_discountedResource(Stone); }},
        {"sale_wood1", [](Player& p, Board&, Player&) { p.set_discountedResource(Wood); }},
        {"sale_clay1", [](Player& p, Board&, Player&) { p.set_discountedResource(Clay); }},
        {"add_resource_glass/papyrus", [](Player& p, Board&, Player&) {
            std::vector<Resource> choices = { Resource::Glass, Resource::Papyrus };
            p.addChoiceResources(choices);
        }},
        {"add_resource_wood/clay/stone", [](Player& p, Board&, Player&) {
        std::vector<Resource> choices = { Resource::Wood, Resource::Clay, Resource::Stone };
        p.addChoiceResources(choices);
        }},

        //Effects for guilds
         {"coins_per_green", [](Player& p, Board&, Player& o) {
            auto countGreen = [](const Player& player) {
             return player.getInventory().count(Color::Green) ? player.getInventory().at(Color::Green).size() : 0;
            };
        int maxGreen = (int)std::max(countGreen(p), countGreen(o));
            p.addResource(Coin, (uint8_t)maxGreen);
             cout << format("{} gained {} coins from Scientists Guild.\n", p.getName(), maxGreen);
         }
        },

        {"coins_per_brown_grey", [](Player& p, Board&, Player& o) {
            auto countBrownGrey = [](const Player& player) {
             int count = 0;
             if (player.getInventory().count(Color::Brown))
                count += (int)player.getInventory().at(Color::Brown).size();
            if (player.getInventory().count(Color::Gray))
                count += (int)player.getInventory().at(Color::Gray).size();
        return count;
         };
    int maxCount = (int)std::max(countBrownGrey(p), countBrownGrey(o));
    p.addResource(Coin, (uint8_t)maxCount);
    cout << p.getName() << " gained " << maxCount
         << " coins from Shipowners Guild." << endl;
        }},

        {"coins_per_yellow", [](Player& p, Board&, Player& o) {
            auto countYellow = [](const Player& player) {
            return player.getInventory().count(Color::Yellow) ? player.getInventory().at(Color::Yellow).size() : 0;
        };
         int maxYellow = (int)std::max(countYellow(p), countYellow(o));
            p.addResource(Coin, (uint8_t)maxYellow);
    cout << p.getName() << " gained " << maxYellow << " coins from Traders Guild." << endl;
        }},

        {"coins_per_blue", [](Player& p, Board&, Player& o) {
            auto countBlue = [](const Player& player) {
        return player.getInventory().count(Color::Blue) ? player.getInventory().at(Color::Blue).size() : 0;
         };
        int maxBlue = (int)std::max(countBlue(p), countBlue(o));
          p.addResource(Coin, (uint8_t)maxBlue);
          cout << p.getName() << " gained " << maxBlue << " coins from Magistrates Guild." << endl;
        }},

    {"coins_per_red", [](Player& p, Board&, Player& o) {
        auto countRed = [](const Player& player) {
         return player.getInventory().count(Color::Red) ? player.getInventory().at(Color::Red).size() : 0;
    };
    int maxRed = (int)std::max(countRed(p), countRed(o));
    p.addResource(Coin, (uint8_t)maxRed);
    cout << p.getName() << " gained " << maxRed
         << " coins from Tacticians Guild." << endl;
     }},

        //Wonder Effects
        {"add_coins3", [](Player& p, Board&, Player&) { p.addResource(Coin,3); }},
        {"decreaseCoins3", [](Player&, Board&, Player& o) { o.decreaseCoins(3); }},
        {"replayTurn", [](Player& p, Board&, Player&) { p.setHasExtraTurn(true); std::cout << "\n[EFECT] " << p.getName() << " a castigat o tura extra!\n"; }},
        {"wood/stone/clay", [](Player& p, Board&, Player&) { p.addResource(Resource::Wood, 1); p.addResource(Resource::Stone, 1); p.addResource(Resource::Clay, 1); }},
        {"add_coins12", [](Player& p, Board&, Player&) { p.addResource(Coin, 12); }},
        {"discardOpponentGrayCard", [](Player&, Board&, Player& o) {
            Game::currentGame->handleDiscardOpponentCardChoice(o, Color::Gray);
}},

{"buildDiscardedCard", [](Player&, Board&, Player&) {
            // Check if an active game instance exists
            if (Game::currentGame) {
                Game::currentGame->handleBuildFromDiscard();
            }
         else {
          std::cerr << "[Error] Game instance not found for Mausoleum effect!" << std::endl;
      }
  }},

{"papyrus/glass", [](Player& p, Board&, Player&) { p.addResource(Resource::Papyrus, 1); p.addResource(Resource::Glass, 1); }},
{"chooseProgressToken", [](Player&, Board&, Player&) {
    Game::currentGame->handleGreatLibraryChoice();
}},
{"chooseProgressTokenFromBox", [](Player&, Board&, Player&) {
    Game::currentGame->handleGreatLibraryChoice();
}},
{"add_coins6", [](Player& p, Board&, Player&) { p.addResource(Coin,6); }},
{"discardOpponentBrownCard", [](Player&, Board&, Player& o) {
    Game::currentGame->handleDiscardOpponentCardChoice(o, Color::Brown);
  }}
};

    while (std::getline(ss, token, ';')) {
        token = trim(token);
        if (token.empty()) continue;

        if (token.find("add_VictoryPoint") == 0) {
            int val = std::stoi(token.substr(16));
            effects.push_back([val](Player& p, Board&, Player&) { p.add_Points(Points::Victory, val); });
        }
        else if (token.find("add_MilitaryPoint") == 0) {
            int val = std::stoi(token.substr(17));
            effects.push_back([val](Player& p, Board& b, Player& o) {
                int take; b.getMilitaryTrack().applyShields(p.getId(), val, take);
                if (take > 0) o.removeResource(Resource::Coin, take);
                });
        }
        else if (effectMap.count(token)) {
            effects.push_back(effectMap.at(token));
        }
    }
    return effects;
}

//translating effects for description

string UniversalCardLoader::translateEffect(const string& raw) {

    if (raw.empty()) return "";
    stringstream ss(raw);
    string token;
    vector<string> results;

    while (getline(ss, token, ';')) {

        token = trim(token);
        if (token.empty())
            continue;
        string trans = token;

        if (token == "add_resource_wood") trans = "🌲 +1 Wood";

        else if (token == "add_resource_clay") trans = "🧱 +1 Clay";

        else if (token == "add_resource_stone") trans = "🪨 +1 Stone";

        else if (token == "add_resource_glass") trans = "🍷 +1 Glass";

        else if (token == "add_resource_papyrus") trans = "📜 +1 Papyrus";

        else if (token == "add_resource_wood2") trans = "🌲 +2 Wood";

        else if (token == "add_resource_clay2") trans = "🧱 +2 Clay";

        else if (token == "add_resource_stone2") trans = "🪨 +2 Stone";

        else if (token == "add_resource_coin4") trans = "💰 +4 Coins";

        else if (token == "add_resource_coin6") trans = "💰 +6 Coins";

        else if (token == "add_scientific_symbol_ink") trans = "✒️ Ink Symbol";

        else if (token == "add_scientific_symbol_scales") trans = "⚖️ Scales Symbol";

        else if (token == "add_scientific_symbol_mortar") trans = "🥣 Mortar Symbol";

        else if (token == "add_scientific_symbol_gyroscope") trans = "🧭 Gyroscope Symbol";

        else if (token == "add_scientific_symbol_sun_dial") trans = "☀️ Sundial Symbol";

        else if (token == "add_scientific_symbol_wheel") trans = "⚙️ Wheel Symbol";

        else if (token == "coin2Wonder") trans = "💰 +2/Wonder";

        else if (token == "coin2Brown") trans = "💰 +2/Brown Card";

        else if (token == "coin1Yellow") trans = "💰 +1/Yellow Card";

        else if (token == "coin3Gray") trans = "💰 +3/Gray Card";

        else if (token == "coin1Red") trans = "💰 +1/Red Card";

        else if (token == "sale_stone1") trans = "💸 Stone Cost: 1";

        else if (token == "sale_wood1") trans = "💸 Wood Cost: 1";

        else if (token == "sale_clay1") trans = "💸 Clay Cost: 1";

        else if (token == "add_resource_glass/papyrus") trans = "🔄 Glass or Papyrus";

        else if (token == "add_resource_wood/clay/stone") trans = "🔄 Wood, Clay, or Stone";

        else if (token.find("add_VictoryPoint") == 0) trans = "🏆 " + token.substr(16) + " VP";

        else if (token.find("add_MilitaryPoint") == 0) trans = "🛡️ " + token.substr(17) + " Shields";

        else if (token == "replayTurn") trans = "🔁 Replay Turn";

        else if (token == "add_coins12") trans = "💰 +12 Coins";

        else if (token == "add_coins6") trans = "💰 +6 Coins";

        else if (token == "add_coins3") trans = "💰 +3 Coins";

        else if (token == "decreaseCoins3") trans = "💸 Opponent -3 Coins";

        else if (token == "wood/stone/clay") trans = "🌲🧱🪨 +1 Each";

        else if (token == "chooseProgressToken") trans = "🧪 Pick Progress Token";
        
        else if (token == "chooseProgressTokenFromBox") trans = "📦 Pick Progress (Box)";

        else if (token == "buildDiscardedCard") trans = "♻️ Build from Discard";

        else if (token == "discardOpponentGrayCard") trans = "🔥 Discard Opp. Gray";

        else if (token == "discardOpponentBrownCard") trans = "🔥 Discard Opp. Brown";

        

        results.push_back(trans);

    }
    string finalStr;
    for (size_t i = 0; i < results.size(); ++i) {

        finalStr += results[i];

        if (i < results.size() - 1) finalStr += " | ";

    }
    return finalStr;

}



//loading

vector<shared_ptr<CardBase>> UniversalCardLoader::loadAgeCards(const string& filename) {

    vector<shared_ptr<CardBase>> cards;

    ifstream file(filename);

    string line;

    getline(file, line); // header



    while (getline(file, line)) {

        if (line.empty()) continue;
        stringstream ss(line);
        string name, id, color, cost, sym, unl, eff, dis;
        getline(ss, name, ','); getline(ss, id, ','); getline(ss, color, ',');
        getline(ss, cost, ','); getline(ss, sym, ','); getline(ss, unl, ',');
        getline(ss, eff, ','); getline(ss, dis, '\n');
                auto card = make_shared<CardBase>(trim(name), (uint16_t)stoi(trim(id)), parseColor(trim(color)), parseCost(trim(cost)), parseSymbol(trim(sym)), parseSymbol(trim(unl)));
                card->m_effectDescription = translateEffect(trim(eff));
                for (auto& ef : parseEffects(trim(eff))) 
                    card->addEffect(ef);
                if (card->getUnlocks().has_value()) 
                {
                    Symbol unlock_symbol = card->getUnlocks().value();
                    card->addEffect([unlock_symbol](Player& p, Board&, Player&) {
                        p.add_ChainSymbol(unlock_symbol);
                    });

                }
                cards.push_back(card);

    }

    return cards;

}



vector<shared_ptr<CardBase>> UniversalCardLoader::loadGuilds(const string& filename) {

    // Guilds have the structure: name,id,cost,symbol,unlocks,effects

    // They are always purple

    vector<shared_ptr<CardBase>> guilds;

    ifstream file(filename);

    string line; getline(file, line);



    while (getline(file, line)) {

        if (line.empty()) continue;

        stringstream ss(line);

        string name, id, cost, sym, unl, eff;

        getline(ss, name, ','); getline(ss, id, ','); getline(ss, cost, ',');

        getline(ss, sym, ','); getline(ss, unl, ','); getline(ss, eff, '\n');



        auto guild = make_shared<CardBase>(trim(name), (uint16_t)stoi(trim(id)), Color::Purple, parseCost(trim(cost)), nullopt, nullopt);

        guild->m_effectDescription = translateEffect(trim(eff));

        for (auto& ef : parseEffects(trim(eff))) guild->addEffect(ef);

        guilds.push_back(guild);

    }

    return guilds;

}



vector<shared_ptr<Wonder>> UniversalCardLoader::loadWonders(const string& filename) {

    vector<shared_ptr<Wonder>> wonders;

    ifstream file(filename);

    string line; getline(file, line);



    while (getline(file, line)) {

        if (line.empty()) continue;

        stringstream ss(line);

        string name, id, cost, eff;

        getline(ss, name, ','); getline(ss, id, ','); getline(ss, cost, ',');

        getline(ss, eff, '\n');



        auto w = make_shared<Wonder>(trim(name), (uint16_t)stoi(trim(id)), parseCost(trim(cost)));

        w->setEffectDescription(translateEffect(trim(eff)));

        for (auto& ef : parseEffects(trim(eff))) w->addEffect(ef);

        wonders.push_back(w);

    }

    return wonders;

}