#include "CardBase.h"
#include<iostream>
#include <format> 
#include <vector>


	const std::string&CardBase::getName() const {
		return m_name;
	}
	std::uint16_t CardBase::getId() const {
		return m_id;
	}
	Color CardBase::getColor() const {
		return m_color;
	}
	const std::map<Resource, std::uint8_t>& CardBase::getCost() const {
		return m_cost;
	}
	const std::optional<Symbol>& CardBase::getSymbol() const {
		return m_symbol;
	}
	const std::optional<Symbol>& CardBase::getUnlocks() const {
		return m_unlocks;
	}
	void CardBase::applyEffect(Player& player,const Player& opponent, const Board & board) const
	{
		for (const auto& effect :m_effects) {
			effect(player);
		}
	}
	void CardBase::addEffect(std::function<void(Player&)> e)
	{
		m_effects.push_back(e);
	}
	void CardBase::setDestroy(std::optional<std::function<void(Player&)>> effect)
	{
		if (effect)
			m_destroy = effect; 
		
	}
	void CardBase::destroy(Player& player) {
		if (m_destroy.has_value()) {
			(*m_destroy)(player);
		}
	}


	std::ostream& operator<<(std::ostream& os, const CardBase& card)
	{
		os << "Card ID: " << card.getId() << ", Name: " << card.getName();
		return os;
	}

	std::string to_string(Resource r)
		{
			switch (r) {
			case Resource::Wood: return "Wood";
			case Resource::Clay: return "Clay";
			case Resource::Stone: return "Stone";
			case Resource::Glass: return "Glass";
			case Resource::Papyrus: return "Papyrus";
			}
			return "Unknown";
		}


	std::string to_string(Color c)
	{
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

	std::string to_string(Symbol s)
		{
			switch (s) {
			case Symbol::Barrel: return "Barrel";
			case Symbol::Gear: return "Gear";
			case Symbol::Sword: return "Sword";
			case Symbol::Horse: return "Horse";
			case Symbol::Helmet: return "Helmet";
			case Symbol::Mask: return "Mask";
			case Symbol::Sun: return "Sun";
			case Symbol::Moon: return "Moon";
	
			}
			return "Unknown";
		}
	
