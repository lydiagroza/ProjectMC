#pragma once
#include "Wonder.h"
#include <vector>
#include <string>
#include <memory>
#include <map>

class WonderLoader {
public:
    static std::map<Resource, uint8_t> parseCost(const std::string& s);
    static std::vector<std::shared_ptr<Wonder>> loadWonders(const std::string& filename);
};