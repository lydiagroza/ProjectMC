#include <iostream>
#include <map>
#include <string>
#include<vector>
#include <optional>
#pragma once
#include <cstdint>
#include <functional>
#include "Board.h"
#include "gameExport.h"
class Player;
class Board;


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
        Victory, Military,BlueCards
	};
  enum Scientific_Symbol : std::uint8_t {
	  Sun_Dial, Wheel, Ink, Mortar,Gyroscope,Scales,Law
  };
  enum class GuildType : uint16_t {
      None = 0,
      Traders = 61,      // Merchants Guild
      Craftsmens = 62,   // Builders Guild
      Philosophers = 63, // Magicians Guild
      Spies = 64,        // Tacticians Guild
      Shipowners = 65    // Shipowners Guild
  };

  class GAME_API CardBase {
  public:

      //Variabile 
      std::string m_name;
      std::uint16_t m_id : 16;
      Color m_color : 3;

      std::optional<Symbol> m_symbol;
      std::optional<Symbol> m_unlocks;
      std::map<Resource, std::uint8_t> m_cost;
      std::vector<std::function<void(Player&,Board&, Player&)>> m_effects;
      std::optional<std::function<void(Player&,Board&, Player &)>> m_destroy; 
      std::string m_effectDescription;



      //Constructori 
      CardBase();
      CardBase(std::string name, std::uint16_t id, Color color,
          const std::map<Resource, std::uint8_t>& cost,
          const std::optional<Symbol>& symbol = std::nullopt,
          const std::optional<Symbol>& unlocks = std::nullopt)
          : m_name(std::move(name)), m_id(id), m_color(color),
           m_symbol(symbol), m_unlocks(unlocks), m_cost(cost) {
      }


      const std::string& getName() const;
      const std::string& getEffectDescription() const;
      std::uint16_t getId() const;
      Color getColor() const;
      const std::map<Resource, std::uint8_t>& getCost() const;
      const std::optional<Symbol>& getSymbol() const;
      const std::optional<Symbol>& getUnlocks() const;
      std::uint8_t getCostForResource(Resource r) const {
          auto it = m_cost.find(r);
          if (it != m_cost.end()) {
              return it->second;
          }
          return 0;
      }

      //Fct pt efecte 
      void applyEffect( Player& player,  Board &board, Player& opponenent)const;
      void addEffect(std::function<void(Player&,Board&,Player &)> e);
      void setDestroy(std::optional<std::function<void(Player&,Board&, Player &) >> e);
      void destroy(Player& player, Board& board, Player& opponent);
    };

  // Operatori , fuctii to_String
    std::ostream& operator<<(std::ostream& os, const CardBase& card);
    std::string to_string(Resource r);
    std::string to_string(Color c);
    std::string to_string(Symbol s);
 