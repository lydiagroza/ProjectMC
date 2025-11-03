module CardBase;
import <iostream>;
import <format>; 
import <vector>;

namespace wonders7 {

	const std::string& CardBase::get_name() const {
		return m_name;
	}

	std::uint16_t CardBase::get_id() const {
		return m_id;
	}

	Points Cardbase::getPoints() const {
		return m_points;
	}
}

