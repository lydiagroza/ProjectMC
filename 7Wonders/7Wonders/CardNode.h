#pragma once
#include <string>
#include <vector>
#include <memory>

class CardBase;


enum class Face {
	Up,
	Down
};
class CardNode
{
private:
	std::shared_ptr<CardBase> m_card; //sa fie shared pointer
	//unique pointer -> numai el poate sterge din heap
	std::vector	<CardNode*> m_children;
	bool m_played;
	Face m_face;

public:
	CardNode();
	void setCard(std::shared_ptr<CardBase> card);
	std::shared_ptr<CardBase> getCard()const;
	bool isPlayed() const;
	bool isPlayable() const;
	std::string getName()const;
	void updatePlayedStatus(bool playedStatus);
	void setFace(Face face);
	void addChild(CardNode* child);
	Face getFace() const;
	const std::vector<CardNode*>& getChildren() const; 

	~CardNode()=default;

};

