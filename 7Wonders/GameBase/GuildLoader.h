#pragma once
#include "CardBase.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <cstdint>
#include <functional>
#include <optional>

class GuildLoader {
public:
    static std::vector<std::shared_ptr<CardBase>> loadFromCSV(const std::string& filename);

private:
    static std::map<Resource, uint8_t> parseCost(const std::string& s);
    static std::vector<std::function<void(Player&, Board&, Player&)>> parseGuildEffects(const std::string& s);
};