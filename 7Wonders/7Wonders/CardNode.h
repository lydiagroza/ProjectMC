#pragma once
#include <string>
#include <vector>

enum class Face {
	Up,
	Down
};
class CardNode
{
private:
	std::string m_nume;
	std::vector	<CardNode> m_children;
	bool m_played;
	bool m_face;
public:
	CardNode(const std::string& nume);
	

};

