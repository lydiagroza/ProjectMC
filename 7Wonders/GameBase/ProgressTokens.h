#pragma once
#include <string>
#include <functional>
#include "gameExport.h" // <-- 1. Include the export header

class Player;

class GAME_API ProgressToken {
    std::string  m_name;
    uint16_t id;
    std::vector<std::function<void(Player&, Player&)>> effects;

public:
    ProgressToken(const std::string& name, uint16_t id)
        : m_name(name), id(id) {
    }


    const std::string& getName() const;
    uint16_t getId() const { return id; }
    const std::vector<std::function<void(Player&, Player&)>>& getEffects() const;

    void addEffect(std::function<void(Player&, Player&)> e);
};

std::ostream& operator<<(std::ostream& os, const ProgressToken& token);