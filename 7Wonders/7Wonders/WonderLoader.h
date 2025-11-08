#pragma once
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include "CardBase.h" 
#include "Player.h"

class Wonder {
    std::string name;
    uint16_t id;
    std::map<Resource, uint8_t> cost;
    std::vector<std::function<void(Player&, Player&)>> effects;

public:
    Wonder(std::string n, uint16_t i, std::map<Resource, uint8_t> c)
        : name(std::move(n)), id(i), cost(std::move(c)) {
    }

    void addEffect(const std::function<void(Player&, Player&)>& f) {
        effects.push_back(f);
    }

    void activate(Player& player, Player& opponent) {
        for (auto& e : effects)
            e(player, opponent);
    }

    const std::string& getName() const { return name; }
};
