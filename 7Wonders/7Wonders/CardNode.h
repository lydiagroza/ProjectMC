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
	std::string m_name;
	std::vector	<CardNode*> m_children;
	bool m_played;
	Face m_face;
public:
	CardNode(const std::string& nume);
	
	bool isPlayable() const;
	std::string getName()const;
	void updatePlayedStatus(bool playedStatus);
	void setFace(Face face);
	void addChild(CardNode* child);

	~CardNode();

};

