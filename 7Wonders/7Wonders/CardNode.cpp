#include "CardNode.h"

CardNode::CardNode(const std::string& nume)
{
	m_nume = nume;
	m_status = false;
	m_face = static_cast<bool>(Face::Down);
}
