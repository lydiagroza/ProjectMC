#pragma once
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include "CardBase.h" 
#include <optional>
#include "CardBase.h"
#include "gameExport.h"

class GAME_API Wonder {
    std::string m_name;
    std::uint16_t m_id;
    std::map<Resource, std::uint8_t> m_cost;
    bool m_isBuilt;
    std::string m_effectDescription;
    std::vector<std::function<void(Player&, Board&, Player&)>> m_effects;
    std::optional<Symbol> m_symbol; // Adaugă membrul pentru simbol
    bool m_isAvailable; // Add m_isAvailable member

public:
    Wonder(std::string name, std::uint16_t id, const std::map<Resource, std::uint8_t>& cost, std::optional<Symbol> symbol = std::nullopt)
        : m_name(std::move(name)), m_id(id), m_cost(cost), m_isBuilt(false), m_symbol(symbol), m_isAvailable(true) {}

    /* static std::map<Resource, uint8_t> parseCost(const std::string& s);
     static std::vector<std::function<void(Player&, Player&)>> parseEffects(const std::string& s);*/

    const std::string& getName() const;
    const std::string& getEffectDescription() const { return m_effectDescription; }
    void setEffectDescription(const std::string& desc) { m_effectDescription = desc; }
    uint16_t getId() const;
        const std::map<Resource, uint8_t>& getCost() const;
    const std::optional<Symbol>& getSymbol() const { return m_symbol; }
    std::vector<std::function<void(Player&, Board&, Player&)>>& getEffects() { return m_effects; }

    const std::vector<std::function<void(Player&,Board&,Player&)>>& getEffects() const;

    std::uint8_t getCostForResource(Resource r) const;

    void addEffect(std::function<void(Player&, Board&, Player&)> e);

    bool getIsBuilt() const;
    void setIsBuilt();
    bool getIsAvailable() const;

    void returnToBox();

};