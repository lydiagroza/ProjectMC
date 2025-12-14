#pragma once
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include "CardBase.h" 

class Player;

class Wonder {
    std::string m_name;
    uint16_t m_id;
    std::map<Resource, uint8_t> m_cost;
    std::vector<std::function<void(Player&, Player&)>> m_effects;
	bool isBuilt = false;

public:
    Wonder(const std::string& name, uint16_t id, const std::map<Resource, uint8_t>& cost)
        : m_name(name), m_id(id), m_cost(cost) {}

   /* static std::map<Resource, uint8_t> parseCost(const std::string& s);
    static std::vector<std::function<void(Player&, Player&)>> parseEffects(const std::string& s);*/

    const std::string& getName() const;
    uint16_t getId() const;
    const std::map<Resource, uint8_t>& getCost() const;
    const std::vector<std::function<void(Player&, Player&)>>& getEffects() const;

    std::uint8_t getCostForResource(Resource r) const;

    void addEffect(std::function<void(Player&, Player&)> e);

    bool getIsBuilt() const;
    void setIsBuilt();

};
