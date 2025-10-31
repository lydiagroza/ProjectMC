import CardBase;
import<iostream>;
import <format>; 
import <vector>;

using namespace wonders7;
using namespace std;

//std::vector<CardBase> createBrownCards() {
//    std::vector<CardBase> cards;

//    // Age I - Brown Cards
//    {
//        CardBase c;
//        c.m_id = 1;
//        c.m_name = "Lumber Yard";
//        c.m_color = Color::Brown;
//        c.m_cost = {};
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 2;
//        c.m_name = "Logging Camp";
//        c.m_color = Color::Brown;
//        c.m_cost = { {Resource::Wood, 1} };
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 3;
//        c.m_name = "Clay Pool";
//        c.m_color = Color::Brown;
//        c.m_cost = {};
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 4;
//        c.m_name = "Clay Pit";
//        c.m_color = Color::Brown;
//        c.m_cost = { {Resource::Clay, 1} };
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 5;
//        c.m_name = "Quarry";
//        c.m_color = Color::Brown;
//        c.m_cost = {};
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 6;
//        c.m_name = "Stone Pit";
//        c.m_color = Color::Brown;
//        c.m_cost = { {Resource::Stone, 1} };
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//
//    // Age II - Brown Cards
//    {
//        CardBase c;
//        c.m_id = 7;
//        c.m_name = "Sawmill";
//        c.m_color = Color::Brown;
//        c.m_cost = { {Resource::Wood, 2} };
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 8;
//        c.m_name = "Brickyard";
//        c.m_color = Color::Brown;
//        c.m_cost = { {Resource::Clay, 2} };
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 9;
//        c.m_name = "Shelf Quarry";
//        c.m_color = Color::Brown;
//        c.m_cost = { {Resource::Stone, 2} };
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//    {
//        CardBase c;
//        c.m_id = 10;
//        c.m_name = "Glass Blower";
//        c.m_color = Color::Brown; // technically Gray, but in image appears as refined resource
//        c.m_cost = {};
//        c.m_symbol = std::nullopt;
//        c.m_unlocks = std::nullopt;
//        cards.push_back(c);
//    }
//
//    return cards;
//}


int main() {

    CardBase test("Lumberyard", 1, Color::Brown, {}, {}, {});
	std::cout << test.get_id() << " " << test.get_name() << "\n";
	return 0;
}
