#ifndef PRODUCTION_CARD_H
#define PRODUCTION_CARD_H

#include "Cards.h"
#include <map>

class ProductionCard : public AgeCard {
    std::map<Resource, int> m_production;

public:
    ProductionCard(
        std::string name,
        int age,
        std::map<Resource, int> cost,
        std::map<Resource, int> production
    );

    void display() const override;
};

#endif
