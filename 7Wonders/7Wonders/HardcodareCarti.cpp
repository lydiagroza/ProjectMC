#include "Player.h"
#include  "CardBase.h"
#include <memory>
#include <map>
#include <vector>

namespace wonders7 {
  /*   m_name(std::move(name)), m_id(id), m_color(color),
        m_cost(cost), m_symbol(symbol), m_unlocks(unlocks)*/

    std::vector<std::unique_ptr<CardBase>> createBrownCards() {
        std::vector<std::unique_ptr<CardBase>> cards;
        auto c = std::make_unique<CardBase>(
            "Lumber Yard",
            1,
            Color::Brown,
            std::map<Resource, std::uint8_t>{{Resource::Wood, 1}}, // cost 
            std::nullopt, // fara simbol 
            std::nullopt  // fara unlock 
        );
          c->addEffect([](Player& p) {
            p.addResource(Resource::Wood, 1);
            });

        cards.push_back(std::move(c));
        return cards;
    }

}
