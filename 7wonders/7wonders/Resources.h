#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <iostream>

enum class Resource {
    Wood,
    Stone,
    Clay,
    Glass,
    Papyrus,
    Coin,
    Military,
    Science,
    Victory
};

inline std::string resourceToString(Resource r) {
    switch (r) {
    case Resource::Wood: return "Wood";
    case Resource::Stone: return "Stone";
    case Resource::Clay: return "Clay";
    case Resource::Glass: return "Glass";
    case Resource::Papyrus: return "Papyrus";
    case Resource::Coin: return "Coin";
    case Resource::Military: return "Military";
    case Resource::Science: return "Science";
    case Resource::Victory: return "Victory";
    default: return "Unknown";
    }
}

inline std::ostream& operator<<(std::ostream& os, Resource r) {
    return os << resourceToString(r);
}

#endif
