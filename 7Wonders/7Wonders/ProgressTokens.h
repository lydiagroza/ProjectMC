#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>

class Player;

class ProgressToken {
    std::string name;
    uint16_t id;
    std::vector<std::function<void(Player&, Player&)>> effects;

public:
    ProgressToken(const std::string& name, uint16_t id)
        : name(name), id(id) {}

    static std::vector<std::function<void(Player&, Player&)>> parseEffects(const std::string& s);

    const std::string& getName() const { return name; }
    uint16_t getId() const { return id; }
    const std::vector<std::function<void(Player&, Player&)>>& getEffects() const { return effects; }

    void addEffect(std::function<void(Player&, Player&)> e) {
        effects.push_back(e);
    }
};