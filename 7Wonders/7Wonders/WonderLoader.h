#pragma once
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include "CardBase.h" 
#include "Player.h"

class Wonder {
    

public:
    std::string name;
    uint16_t id;
    static std::map<Resource, uint8_t> parseCost(const std::string& s);
    static std::vector<std::function<void(Player&, Player&)>> ParseEffects(const std::string& s);
   
    const std::string& getName() const { return name; }
    uint16_t getId() const { return id; }

    void addEffect(std::function<void(Player&)> e) {
        effects.push_back(e);
    }

};
