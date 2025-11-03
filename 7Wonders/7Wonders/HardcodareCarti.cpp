#include "Player.h"
#include  "CardBase.h"
#include <memory>
#include <map>
#include <vector>

namespace wonders7 {

    std::vector<std::unique_ptr<CardBase>> createBrownCards() {
        std::vector<std::unique_ptr<CardBase>> cards;
      
        // LUMBERYARD  
        auto c = std::make_unique<CardBase>(
            "Lumber Yard",
            1,
            Color::Brown,
            std::map<Resource, std::uint8_t>{}, // cost 
            std::nullopt, // fara simbol 
            std::nullopt  // fara unlock 
        );
        c->addEffect([](Player& p) {
            p.addResource(Resource::Wood, 1);
            });

        cards.push_back(std::move(c));



		// LOGGING CAMP
        auto c = std::make_unique<CardBase>(
            "Logging Camp",
            2,
            Color::Brown,
            std::map<Resource, std::uint8_t>{{Resource::Coin, 1}}, // cost 
            std::nullopt, // fara simbol 
            std::nullopt  // fara unlock 
        );
        c->addEffect([](Player& p) {
            p.addResource(Resource::Wood, 1);
            });
        cards.push_back(std::move(c));



		// CLAY PIT
        auto c = std::make_unique<CardBase>(
            "Clay Pit", 3,
            Color::Brown,
            std::map<Resource, std::uint8_t>{}, // cost 
            std::nullopt, // fara simbol 
            std::nullopt  // fara unlock 
        );
        c->addEffect([](Player& p) {
            p.addResource(Resource::Clay, 1);
            });
        cards.push_back(std::move(c));



		// CLAY POOL
        auto c = std::make_unique<CardBase>(
            "Clay Pool", 4,
            Color::Brown,
            std::map<Resource, std::uint8_t>{{Resource::Coin, 1}}, // cost 
            std::nullopt, // fara simbol 
            std::nullopt  // fara unlock 
        );
        c->addEffect([](Player& p) {
            p.addResource(Resource::Clay, 1);
            });
        cards.push_back(std::move(c));




		// STONE PIT
        auto c = std::make_unique<CardBase>(
            "Stone Pit", 5,
            Color::Brown,
            std::map<Resource, std::uint8_t>{{Resource::Coin, 1}}, // cost 
            std::nullopt, // fara simbol 
            std::nullopt  // fara unlock 
        );
        c->addEffect([](Player& p) {
            p.addResource(Resource::Stone, 1);
            });
        cards.push_back(std::move(c));



		// GLASS WORKS
        auto c = std::make_unique<CardBase>(
            "Glass Works", 6,
            Color::Gray,
            std::map<Resource, std::uint8_t>{{Resource::Coin, 1}}, // cost 
            std::nullopt, // fara simbol 
            std::nullopt  // fara unlock 
        );
        c->addEffect([](Player& p) {
            p.addResource(Resource::Glass, 1);
            });
        cards.push_back(std::move(c));



		// PRESS
        auto c = std::make_unique<CardBase>(
            "Press", 7,
            Color::Gray,
            std::map<Resource, std::uint8_t>{{Resource::Coin, 1}}, // cost 
            std::nullopt, // fara simbol 
            std::nullopt  // fara unlock 
        );
        c->addEffect([](Player& p) {
            p.addResource(Resource::Papyrus, 1);
            });
        cards.push_back(std::move(c));



		// GUARD TOWER
        auto c = std::make_unique<CardBase>(
            "Guard Tower", 8,
            Color::Red,
            std::map<Resource, std::uint8_t>{}, // cost 
            std::nullopt, // fara simbol 
            std::nullopt  // fara unlock 
        );
        c->addEffect([](Player& p) {
            p.add_Points(Points::Military, 1);
            });
        cards.push_back(std::move(c));



		// WORKSHOP
        auto c = std::make_unique<CardBase>(
            "Workshop", 9,
            Color::Green,
            std::map<Resource, std::uint8_t>{{Resource::Papyrus, 1}}, // cost 
            std::nullopt,// fara simbol 
            std::nullopt  // fara unlock 
        );
        c->addEffect([](Player& p) {
            p.add_ScientificSymbol(Scientific_Symbol::Cog);
			p.add_Points(Points::Victory, 1);
            });
        cards.push_back(std::move(c));



		// APOTHECARY
        auto c = std::make_unique<CardBase>(
            "Apothecary", 10,
            Color::Green,
            std::map<Resource, std::uint8_t>{}, // cost 
            std::nullopt,// fara simbol 
            std::nullopt  // fara unlock 
        );
        c->addEffect([](Player& p) {
            p.add_ScientificSymbol(Scientific_Symbol::Wheel);
            p.add_Points(Points::Victory, 1);
            });
        cards.push_back(std::move(c));



	return cards;

}
