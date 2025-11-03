#include "CardBase.h"
#include<iostream>
#include <format> 
#include <vector>


	const std::string&CardBase::get_name() const {
		return m_name;
	}
	std::uint16_t CardBase::get_id() const {
		return m_id;
	}
	Color CardBase::get_color() const {
		return m_color;
	}
	const std::map<Resource, std::uint8_t>& CardBase::get_cost() const {
		return m_cost;
	}
	const std::optional<Symbol>& CardBase::get_symbol() const {
		return m_symbol;
	}
	const std::optional<std::vector<Symbol>>& CardBase::get_unlocks() const {
		return m_unlocks;
	}
	void CardBase::applyEffect(Player& p)
	{
		for (const auto& effect : effects) {
			effect(p);
		}
	}
	void CardBase::addEffect(std::function<void(Player&)> e)
	{
		effects.push_back(e);
	}
	std::ostream& operator<<(std::ostream& os, const CardBase& card)
	{
		os << "Card ID: " << card.get_id() << ", Name: " << card.get_name();
		return os;
	}
