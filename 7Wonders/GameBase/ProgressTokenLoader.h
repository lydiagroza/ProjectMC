#pragma once
#include "ProgressTokens.h"
#include <vector>
#include <string>
#include <memory>

class ProgressTokenLoader {
public:
    static std::vector<std::function<void(Player&, Player&)>> parseEffects(const std::string& s);
    static std::vector<std::shared_ptr<ProgressToken>> loadProgressTokens(const std::string& filename);
};