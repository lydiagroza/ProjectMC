#include "CardNode.h"
#include <string>

CardNode::CardNode(const std::string& nume) : m_name(nume), m_played(false), m_face(Face::Down)
{
}

bool CardNode::isPlayable() const
{
	if(Face::Up == m_face && !m_played && m_children.empty())
	{
		return true;
	}
	return false;
}

std::string CardNode::getName() const
{
	return m_name;
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
