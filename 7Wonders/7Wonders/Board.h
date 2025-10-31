#pragma once
#include <vector>

class Board
{
	class Node {
	private:
		int value;
		std::vector<Node> children;
	public:
		Node(int value)
		{
			this->value = value;
			this->children = std::vector<Node>();
		}

		void addChild(int value)
		{
			this->children.push_back(Node(value));
		}
		int getValue() const
		{
			return this->value;
		}

	};



};

