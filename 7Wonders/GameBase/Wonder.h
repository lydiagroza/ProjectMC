#pragma once
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include "CardBase.h" 
#include "gameExport.h"

class Player;


//cam pt m_isAvailable si un getter pt ea si o fc de return toBox care steabileste isAvailable pe false
// -> ca sa nu mai poata fi construita

class GAME_API Wonder {
    std::string m_name;
    uint16_t m_id;
    std::map<Resource, uint8_t> m_cost;
    std::vector<std::function<void(Player&, Player&)>> m_effects;
	bool m_isBuilt = false;
    bool m_isAvailable = true;

public:
    Wonder(const std::string& name, uint16_t id, const std::map<Resource, uint8_t>& cost)
        : m_name(name), m_id(id), m_cost(cost) {}

    static std::map<Resource, uint8_t> parseCost(const std::string& s);
    static std::vector<std::function<void(Player&, Player&)>> parseEffects(const std::string& s);

    const std::string& getName() const { return m_name; }
    uint16_t getId() const { return m_id; }
    const std::map<Resource, uint8_t>& getCost() const { return m_cost; }
    const std::vector<std::function<void(Player&, Player&)>>& getEffects() const { return m_effects; }

    std::uint8_t getCostForResource(Resource r) const {
        auto it = m_cost.find(r);
        if (it != m_cost.end()) {
            return it->second;
        }
        return 0;
    }

	bool getIsBuilt() const { return m_isBuilt; }
    bool getIsAvailable() const { return m_isAvailable; }


	void setIsBuilt() { 
        if (m_isAvailable) {
            m_isBuilt = true;
        }
        else {
            // punem eroare? asta este pentru cand minunea a fost eliminata 
        }
    }

    void returnToBox() {
        m_isAvailable = false;
    }


    void addEffect(std::function<void(Player&, Player&)> e) {
        m_effects.push_back(e);
    }

};
