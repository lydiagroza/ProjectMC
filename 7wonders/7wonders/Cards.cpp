#include "Cards.h"
#include <iostream>

AgeCard::AgeCard(std::string name, int age, std::map<Resource, int> cost)
    : m_name(std::move(name)), m_age(age), m_cost(std::move(cost)) {
}

void AgeCard::display() const {
    std::cout << "Card: " << m_name << " | Age: " << m_age << "\n";
    if (!m_cost.empty()) {
        std::cout << "  Cost: ";
        for (const auto& [res, qty] : m_cost)
            std::cout << res << "(" << qty << ") ";
        std::cout << "\n";
    }
}
