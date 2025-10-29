export module AgeCards;
import <iostream>;
import <string>;

namespace wonders7
{
	export class AgeCards {
	public:

			enum class Category : std::uint8_t {
			Brown,  // common resources
			Grey,   // rare resources
			Blue,   // provide victory points 
			Yellow, // commercial advantages
			Purple, // only in age 3 victory points 
			Red,    // military strength
			Green,  // scientific advancements
			Black   // cities buildings
		};

		enum class BrownType : std::uint8_t {
			Clay,
			Wood,
			Stone,
			Ore
		};

		enum class GreyType : std::uint8_t {
			Glass,
			Cloth,
			Papyrus
		};

		// Constructor
		AgeCards(Category category, int cost, int age)
			: m_category(category), m_cost(cost), m_availableAge(age) {
		}

		// Getters
		Category getCategory() const { return m_category; }
		int getCost() const { return m_cost; }
		int getAvailableAge() const { return m_availableAge; }

	private:
		Category m_category : 3;
		int m_cost;
		int m_availableAge;
	};

	// Operator << pentru debugging / afișare
	std::ostream& operator<<(std::ostream& os, const AgeCards& card)
	{
		using Category = AgeCards::Category;
		switch (card.getCategory()) {
		case Category::Brown:  os << "Brown"; break;
		case Category::Grey:   os << "Grey"; break;
		case Category::Blue:   os << "Blue"; break;
		case Category::Yellow: os << "Yellow"; break;
		case Category::Purple: os << "Purple"; break;
		case Category::Red:    os << "Red"; break;
		case Category::Green:  os << "Green"; break;
		case Category::Black:  os << "Black"; break;
		}
		os << " Card | Cost: " << card.getCost() << " | Age: " << card.getAvailableAge();
		return os;
	}

} // namespace wonders7
