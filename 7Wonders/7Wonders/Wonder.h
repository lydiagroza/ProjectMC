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
	bool isBuilt = false;

public:
    Wonder(const std::string& name, uint16_t id, const std::map<Resource, uint8_t>& cost)
        : name(name), id(id), cost(cost) {}

    static std::map<Resource, uint8_t> parseCost(const std::string& s);
    static std::vector<std::function<void(Player&, Player&)>> parseEffects(const std::string& s);

    const std::string& getName() const { return name; }
    uint16_t getId() const { return id; }
    const std::map<Resource, uint8_t>& getCost() const { return cost; }
    const std::vector<std::function<void(Player&, Player&)>>& getEffects() const { return effects; }

    void addEffect(std::function<void(Player&, Player&)> e) {
        effects.push_back(e);
    }

	bool getIsBuilt() const { return isBuilt; }
	void setIsBuilt() { isBuilt = true; }
};
