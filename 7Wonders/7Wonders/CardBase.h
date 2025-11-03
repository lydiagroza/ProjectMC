#include <iostream>
#include <map>
#include <string>
#include<vector>
#include <optional>
#include<functional>
#include "Player.h" 
#include <cstdint>


  enum  Resource : std::uint8_t {
        Wood, Clay, Stone, Glass, Papyrus, Coin
    };

  enum Color : std::uint8_t {
        Brown, Gray, Yellow, Red, Blue, Green, Purple
    };

  enum  Symbol : std::uint8_t {
        Barrel, Gear, Lamp, Scroll, Sword, Wall,
        Horse, Helmet, Mask, Sun, Moon, Balance
    };
  enum  Points : std::uint8_t {
        Victory, War
	};


  class CardBase {
  public:
      std::string m_name;
      std::uint16_t m_id : 16;
      Color m_color : 3;
      std::optional<Symbol> m_symbol;
      std::optional<std::vector<Symbol>> m_unlocks;
      std::map<Resource, std::uint8_t> m_cost;
      std::vector<std::function<void(Player&)>> effects;
      CardBase();
      CardBase(std::string name, std::uint16_t id, Color color,
          const std::map<Resource, std::uint8_t>& cost,
          const std::optional<Symbol>& symbol = std::nullopt,
          const std::optional<std::vector<Symbol>>& unlocks = std::nullopt)
          : m_name(std::move(name)), m_id(id), m_color(color),
          m_cost(cost), m_symbol(symbol), m_unlocks(unlocks) {
      }
      const std::string& get_name() const;
      std::uint16_t get_id() const;
      Color get_color() const;
      const std::map<Resource, std::uint8_t>& get_cost() const;
      const std::optional<Symbol>& get_symbol() const;
      const std::optional<std::vector<Symbol>>& get_unlocks() const;
      void applyEffect(Player& p);
      void addEffect(std::function<void(Player&)> e);
    };


    std::ostream& operator<<(std::ostream& os, const CardBase& card);
std::string to_string(Resource r) {
        switch (r) {
        case Resource::Wood: return "Wood";
        case Resource::Clay: return "Clay";
        case Resource::Stone: return "Stone";
        case Resource::Glass: return "Glass";
        case Resource::Papyrus: return "Papyrus";
        case Resource::Gold: return "Gold";
        }
        return "Unknown";
    }

std::string to_string(Color c) {
        switch (c) {
        case Color::Brown: return "Brown";
        case Color::Gray: return "Gray";
        case Color::Yellow: return "Yellow";
        case Color::Red: return "Red";
        case Color::Blue: return "Blue";
        case Color::Green: return "Green";
        case Color::Purple: return "Purple";
        }
        return "Unknown";
    }

 std::string to_string(Symbol s) {
        switch (s) {
        case Symbol::Barrel: return "Barrel";
        case Symbol::Gear: return "Gear";
        case Symbol::Lamp: return "Lamp";
        case Symbol::Scroll: return "Scroll";
        case Symbol::Sword: return "Sword";
        case Symbol::Wall: return "Wall";
        case Symbol::Horse: return "Horse";
        case Symbol::Helmet: return "Helmet";
        case Symbol::Mask: return "Mask";
        case Symbol::Sun: return "Sun";
        case Symbol::Moon: return "Moon";
        case Symbol::Balance: return "Balance";
        }
        return "Unknown";
    }
std::ostream& operator<<(std::ostream& os, const CardBase& card) {
        os << "Card Name: " << card.m_name << "\n";
        os << "ID: " << card.m_id << "\n";
        os << "Color: " << to_string(card.m_color) << "\n";

        // Cost
        if (card.m_cost.empty()) {
            os << "Cost: None\n";
        }
        else {
            os << "Cost: ";
            bool first = true;
            for (const auto& [res, val] : card.m_cost) {
                if (!first) os << ", ";
                os << (int)val << "x " << to_string(res);
                first = false;
            }
            os << "\n";
        }

        // Symbol
        if (card.m_symbol)
            os << "Symbol: " << to_string(*card.m_symbol) << "\n";
        else
            os << "Symbol: None\n";

        // Unlocks
        if (card.m_unlocks && !card.m_unlocks->empty()) {
            os << "Unlocks: ";
            bool first = true;
            for (auto sym : *card.m_unlocks) {
                if (!first) os << ", ";
                os << to_string(sym);
                first = false;
            }
            os << "\n";
        }
        else {
            os << "Unlocks: None\n";
        }

        os << "-----------------------------\n";
        return os;
    }


