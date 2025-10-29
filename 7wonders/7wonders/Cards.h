#ifndef CARD_H
#define CARD_H

#include <string>
#include <map>
#include "Resources.h"

class AgeCard {
protected:
    std::string m_name;
    int m_age;
    std::map<Resource, int> m_cost;

public:
    AgeCard(std::string name, int age, std::map<Resource, int> cost = {});
    virtual ~AgeCard() = default;

    virtual void display() const;
};

#endif
