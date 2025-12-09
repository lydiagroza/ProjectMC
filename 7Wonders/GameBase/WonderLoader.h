#pragma once
#include "Wonder.h"
#include <vector>
#include <string>
#include <memory>

class WonderLoader {
public:
    static std::vector<std::shared_ptr<Wonder>> loadWonders(const std::string& filename);
};
