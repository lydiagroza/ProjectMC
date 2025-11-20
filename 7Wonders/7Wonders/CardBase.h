#include <iostream>
#include <map>
#include <string>
#include<vector>
#include <optional>
#pragma once
#include <cstdint>
#include <functional>
#include "Board.h"

// Enumuri 
  enum  Resource : std::uint8_t {
        Wood, Clay, Stone, Glass, Papyrus, Coin
    };

  enum Color : std::uint8_t {
        Brown, Gray, Yellow, Red, Blue, Green, Purple
    };

  enum  Symbol : std::uint8_t {
      Barrel, Gear, Book, Temple, Target, Lyre, Castle, Droplet, Vase, Column, Sword, Pot, Horse, Helmet, Mask, Sun, Moon
    };
  enum  Points : std::uint8_t {
        Victory, Military
	};
  enum Scientific_Symbol : std::uint8_t {
	  Sun_Dial, Wheel, Ink, Mortar,Gyroscope,Scales
  };

  class CardBase {
  public:

      //Variabile 
      std::string m_name;
      std::uint16_t m_id : 16;
      Color m_color : 3;
      std::optional<Symbol> m_symbol;
      std::optional<Symbol> m_unlocks;
      std::map<Resource, std::uint8_t> m_cost;
      std::vector<std::function<void(Player&)>> m_effects;
      bool m_isAvailable; 

      //Constructori 
      CardBase();
      CardBase(std::string name, std::uint16_t id, Color color,
          const std::map<Resource, std::uint8_t>& cost,
          const std::optional<Symbol>& symbol = std::nullopt,
          const std::optional<Symbol>& unlocks = std::nullopt)
          : m_name(std::move(name)), m_id(id), m_color(color),
          m_cost(cost), m_symbol(symbol), m_unlocks(unlocks),m_isAvailable(false) {
      }

      // Getteri 
      const std::string& get_name() const;
      std::uint16_t get_id() const;
      Color get_color() const;
      const std::map<Resource, std::uint8_t>& get_cost() const;
      const std::optional<Symbol>& get_symbol() const;
      const std::optional<Symbol>& get_unlocks() const;
      std::uint8_t getCostForResource(Resource r) const {
          auto it = m_cost.find(r);
          if (it != m_cost.end()) {
              return it->second;
          }
          return 0;
      }

      //Fct pt efecte 
      void applyEffect(Player& player,Player& opponenent)const;
      void addEffect(std::function<void(Player&)> e);
    };

  // Operatori , fuctii to_String
    std::ostream& operator<<(std::ostream& os, const CardBase& card);
    std::string to_string(Resource r);
    std::string to_string(Color c);
    std::string to_string(Symbol s);
 


    class Player; 