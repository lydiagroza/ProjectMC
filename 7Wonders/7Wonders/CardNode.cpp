#include "CardNode.h"


CardNode::CardNode() : m_card(nullptr), m_played(false), m_face(Face::Down)
{
}

void CardNode::setCard(CardBase* card)
{
	m_card = card;
}

CardBase* CardNode::getCard() const
{
	return m_card;
}

bool CardNode::isPlayable() const
{
    if (m_played) 
        return false;
    if (m_face != Face::Up)
        return false;

    for (const auto* child : m_children) {
        if (child->m_played == false)
            return false;
    }

    return true;
}

std::string CardNode::getName() const
{
	if (m_card != nullptr) {
		return m_card->get_name();
	}
	return "Empty Node";
}

void CardNode::updatePlayedStatus(bool playedStatus)
{
	m_played = playedStatus;
}

void CardNode::setFace(Face face)
{
	m_face = face;
}

void CardNode::addChild(CardNode* child) {
    m_children.push_back(child);
}

Face CardNode::getFace() const
{
	return m_face;
}

const std::vector<CardNode*>& CardNode::getChildren() const
{
	return m_children;
}
