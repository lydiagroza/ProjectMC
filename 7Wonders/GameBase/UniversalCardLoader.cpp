#include "UniversalCardLoader.h"
#include "Game.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace std;

// --- UTILITARE ---
string UniversalCardLoader::trim(string s) {
    s.erase(remove(s.begin(), s.end(), '"'), s.end()); // Elimină ghilimelele
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

        int quantity = 1;
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
    static const unordered_map<string, Symbol> symMap = {
        {"barrel", Barrel}, {"gear", Gear}, {"book", Book}, {"temple", Temple},
        {"target", Target}, {"lyre", Lyre}, {"castle", Castle}, {"droplet", Droplet},
        {"vase", Vase}, {"column", Column}, {"sword", Sword}, {"pot", Pot},
        {"horse", Horse}, {"helmet", Helmet}, {"mask", Mask}, {"sun", Sun}, {"moon", Moon}
    };
    auto it = symMap.find(s);
    return (it != symMap.end()) ? make_optional(it->second) : nullopt;
}

// --- EFECTE ---
vector<function<void(Player&, Board&, Player&)>> UniversalCardLoader::parseEffects(const string& s) {
    vector<function<void(Player&, Board&, Player&)>> effects;
    if (s.empty()) return effects;

    stringstream ss(s);
    string token;

    static const std::unordered_map<std::string, std::function<void(Player&, Board&, Player&)>> effectMap = {
        //Efectele pentru cele 3 age-uri
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
        {"add_scientific_symbol_wheel", [](Player& p,Board& b, Player&) { p.add_ScientificSymbol(Scientific_Symbol::Wheel); }},
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
        std::vector<Resource> choices = { Resource::Wood, Resource::Clay, Resource::Stone };
        p.addChoiceResources(choices);
        }},

        //Efecte pentru guilds
         {"coins_per_green", [](Player& p, Board& b, Player& o) {
            auto countGreen = [](const Player& player) {
             return player.getInventory().count(Color::Green) ? player.getInventory().at(Color::Green).size() : 0;
            };
        int maxGreen = std::max(countGreen(p), countGreen(o));
            p.addResource(Coin, maxGreen);
             cout << p.getName() << " gained " << maxGreen << " coins from Scientists Guild." << endl;
         }
        },

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
            return player.getInventory().count(Color::Yellow) ? player.getInventory().at(Color::Yellow).size() : 0;
        };
         int maxYellow = std::max(countYellow(p), countYellow(o));
            p.addResource(Coin, maxYellow);
    cout << p.getName() << " gained " << maxYellow << " coins from Traders Guild." << endl;
        }},

        {"coins_per_blue", [](Player& p, Board& b, Player& o) {
            auto countBlue = [](const Player& player) {
        return player.getInventory().count(Color::Blue) ? player.getInventory().at(Color::Blue).size() : 0;
         };
        int maxBlue = std::max(countBlue(p), countBlue(o));
          p.addResource(Coin, maxBlue);
          cout << p.getName() << " gained " << maxBlue << " coins from Magistrates Guild." << endl;
        }},

    {"coins_per_red", [](Player& p, Board& b, Player& o) {
        auto countRed = [](const Player& player) {
         return player.getInventory().count(Color::Red) ? player.getInventory().at(Color::Red).size() : 0;
    };
    int maxRed = std::max(countRed(p), countRed(o));
    p.addResource(Coin, maxRed);
    cout << p.getName() << " gained " << maxRed
         << " coins from Tacticians Guild." << endl;
     }},

        //Efecte Wonders
        {"add_coins3", [](Player& p,Board& b, Player& o) { p.addResource(Coin,3); }},
        {"decreaseCoins3", [](Player& p,Board& b, Player& o) { o.decreaseCoins(3); }},
        {"replayTurn", [](Player& p,Board& b, Player& o) { p.setHasExtraTurn(true); std::cout << "\n[EFECT] " << p.getName() << " a castigat o tura extra!\n"; }},
        {"wood/stone/clay", [](Player& p,Board& b, Player& o) { p.addResource(Resource::Wood, 1); p.addResource(Resource::Stone, 1); p.addResource(Resource::Clay, 1); }},
        {"add_coins12", [](Player& p,Board& b, Player& o) { p.addResource(Coin, 12); }},
        {"discardOpponentGrayCard", [](Player& p,Board& b, Player& o) {
            Board& board = Game::currentGame->getBoard();
            const auto& opponentInventory = o.getInventory();
            std::vector<std::shared_ptr<CardBase>> grayCards;
            if (opponentInventory.count(Color::Gray)) {
                grayCards = opponentInventory.at(Color::Gray);
            }

            if (grayCards.empty()) {
                std::cout << "Opponent has no gray cards to discard." << std::endl;
                return;
            }

            std::cout << "Choose a gray card to discard from your opponent:" << std::endl;
            for (int i = 0; i < (int)grayCards.size(); ++i) {
                std::cout << i + 1 << ": " << *grayCards[i] << std::endl;
            }

            int choice = -1;
            // UI should provide choice; placeholder here
            if (choice > 0 && choice <= (int)grayCards.size()) {
                auto selectedCard = grayCards[choice - 1];
                o.removeCardFromInventory(selectedCard);
                board.addCardToDiscardPile(selectedCard);
                std::cout << "Discarded " << selectedCard->getName() << " from opponent." << std::endl;
            }
 else {
  std::cout << "No choice made (UI required) or invalid choice." << std::endl;
}
}},

{"buildDiscardedCard", [](Player& p,Board& b,Player& o) {
            // Verificăm dacă există o instanță de joc activă
            if (Game::currentGame) {
                // Apelăm funcția pe care ai scris-o deja în clasa Game
                Game::currentGame->handleBuildFromDiscard();
            }
         else {
          std::cerr << "[Eroare] Instanta Game nu a fost gasita pentru efectul Mausoleum!" << std::endl;
      }
  }},

{"papyrus/glass", [](Player& p,Board& b, Player& o) { p.addResource(Resource::Papyrus, 1); p.addResource(Resource::Glass, 1); }},
{"chooseProgressToken", [](Player& p,Board& b, Player& o) {
    Board& board = Game::currentGame->getBoard();
    auto availableTokens = board.getAvailableProgressTokens();

    if (availableTokens.empty()) {
        std::cout << "No available progress tokens." << std::endl;
        return;
    }

    std::cout << "Choose a progress token:" << std::endl;
    for (int i = 0; i < (int)availableTokens.size(); ++i) {
        std::cout << i + 1 << ": " << *availableTokens[i] << std::endl;
    }

    int choice = -1;
    // UI should provide choice; placeholder here

    if (choice > 0 && choice <= (int)availableTokens.size()) {
        auto selectedToken = availableTokens[choice - 1];
        for (const auto& effect : selectedToken->getEffects()) {
            effect(p, o);
        }
        board.removeAvailableProgressToken(selectedToken);
        std::cout << "You chose the " << selectedToken->getName() << " token." << std::endl;
    }
else {
 std::cout << "No choice made (UI required) or invalid choice." << std::endl;
}
}},
{"add_coins6", [](Player& p,Board& b, Player& o) { p.addResource(Coin,6); }},
{"discardOpponentBrownCard", [](Player& p,Board& b, Player& o) {
    Board& board = Game::currentGame->getBoard();
    auto opponentInventory = o.getInventory();
    std::vector<std::shared_ptr<CardBase>> brownCards;
    if (opponentInventory.count(Color::Brown)) {
        brownCards = opponentInventory.at(Color::Brown);
    }

    if (brownCards.empty()) {
        std::cout << "Opponent has no brown cards to discard." << std::endl;
        return;
    }

    std::cout << "Choose a brown card to discard from your opponent:" << std::endl;
    for (int i = 0; i < (int)brownCards.size(); ++i) {
        std::cout << i + 1 << ": " << *brownCards[i] << std::endl;
    }

    int choice = -1;
    // UI should provide choice; placeholder here

    if (choice > 0 && choice <= (int)brownCards.size()) {
        auto selectedCard = brownCards[choice - 1];
        o.removeCardFromInventory(selectedCard);
        board.addCardToDiscardPile(selectedCard);
        std::cout << "Discarded " << selectedCard->getName() << " from opponent." << std::endl;
    }
else {
 std::cout << "No choice made (UI required) or invalid choice." << std::endl;
    }
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

// --- LOADING ---
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
        for (auto& ef : parseEffects(trim(eff))) card->addEffect(ef);
        cards.push_back(card);
    }
    return cards;
}

vector<shared_ptr<CardBase>> UniversalCardLoader::loadGuilds(const string& filename) {
    // Gildele au structura: name,id,cost,symbol,unlocks,effects
    // Sunt mereu mov (Purple)
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
        getline(ss, name, ','); getline(ss, id, ','); getline(ss, cost, ','); getline(ss, eff, '\n');

        auto w = make_shared<Wonder>(trim(name), (uint16_t)stoi(trim(id)), parseCost(trim(cost)));
        for (auto& ef : parseEffects(trim(eff))) w->addEffect(ef);
        wonders.push_back(w);
    }
    return wonders;
}