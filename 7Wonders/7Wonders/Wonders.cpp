#include "Wonder.h"
#include "Player.h"
#include <sstream>
#include <unordered_map>
#include <iostream>
#include "CardLoader.h"
#include "Game.h"

#include <memory>
class Player;


const std::string & Wonder::getName() const {
	return m_name; 
}
uint16_t Wonder :: getId() const { return m_id; }
const std::map<Resource, uint8_t>& Wonder :: getCost() const { return m_cost; }
const std::vector<std::function<void(Player&, Player&)>>& Wonder:: getEffects() const { return m_effects; }

std::uint8_t getCostForResource(Resource r) const {
    auto it = m_cost.find(r);
    if (it != m_cost.end()) {
        return it->second;
    }
    return 0;
}