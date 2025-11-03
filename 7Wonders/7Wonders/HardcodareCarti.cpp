#include "CardBase.h"
using namespace std;

//std::vector<CardBase> createBrownCards() {
//    std::vector<CardBase> cards;

//    // Age I - Brown Cards
//    {
//        CardBase c;
//        c.m_id = 1;
//        c.m_name = "Lumber Yard";
//        c.m_color = Color::Brown;
//        c.m_cost = {};
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 2;
//        c.m_name = "Logging Camp";
//        c.m_color = Color::Brown;
//        c.m_cost = { {Resource::Wood, 1} };
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 3;
//        c.m_name = "Clay Pool";
//        c.m_color = Color::Brown;
//        c.m_cost = {};
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 4;
//        c.m_name = "Clay Pit";
//        c.m_color = Color::Brown;
//        c.m_cost = { {Resource::Clay, 1} };
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 5;
//        c.m_name = "Quarry";
//        c.m_color = Color::Brown;
//        c.m_cost = {};
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 6;
//        c.m_name = "Stone Pit";
//        c.m_color = Color::Brown;
//        c.m_cost = { {Resource::Stone, 1} };
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//
//    // Age II - Brown Cards
//    {
//        CardBase c;
//        c.m_id = 7;
//        c.m_name = "Sawmill";
//        c.m_color = Color::Brown;
//        c.m_cost = { {Resource::Wood, 2} };
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 8;
//        c.m_name = "Brickyard";
//        c.m_color = Color::Brown;
//        c.m_cost = { {Resource::Clay, 2} };
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 9;
//        c.m_name = "Shelf Quarry";
//        c.m_color = Color::Brown;
//        c.m_cost = { {Resource::Stone, 2} };
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 10;
//        c.m_name = "Glass Blower";
//        c.m_color = Color::Brown; // technically Gray, but in image appears as refined resource
//        c.m_cost = {};
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//
//    return cards;
//}

import CardBase;
import <iostream>;

using namespace wonders7;

int main() {
    CardBase c1("Baths", 1, Color::Blue,
        { {Resource::Stone, 1} },
        Symbol::Lamp);

    CardBase c2("Guard Tower", 2, Color::Red,
        { {Resource::Clay, 1} },
        Symbol::Sword);

    CardBase c3("Marketplace", 3, Color::Yellow,
        { {Resource::Gold, 2} },
        std::nullopt,
        std::vector<Symbol>{Symbol::Gear, Symbol::Scroll});

    std::cout << c1 << c2 << c3;
}
