#include "ProductionCards.h"
#include <iostream>

ProductionCard::ProductionCard(
    std::string name,
    int age,
    std::map<Resource, int> cost,
    std::map<Resource, int> production
)
    : AgeCard(std::move(name), age, std::move(cost)), m_production(std::move(production)) {
}

void ProductionCard::display() const {
    AgeCard::display();
    if (!m_production.empty()) {
        std::cout << "  Produces: ";
        for (const auto& [res, qty] : m_production)
            std::cout << res << "(" << qty << ") ";
        std::cout << "\n";
    }
}
