#ifndef PACKINGTREENODE_H
#define PACKINGTREENODE_H

#include "BinaryTree.h"
#include "SFML/Graphics.hpp"

class PackingTreeNode
{
public:
	sf::IntRect * binRectangle;
	PackingTreeNode * leftChild;
	PackingTreeNode * rightChild;
	bool filled;
	sf::Color color;

	PackingTreeNode(sf::IntRect * binRectangle)
	{
		this->binRectangle = binRectangle;
		leftChild = NULL;
		rightChild = NULL;
		filled = false;

		color.r = sf::Randomizer::Random(0, 255);
		color.g = sf::Randomizer::Random(0, 255);
		color.b = sf::Randomizer::Random(0, 255);
	}

	sf::IntRect * insert(sf::IntRect * inputRectangle)
	{
		if(leftChild != NULL || rightChild != NULL)
		{
			sf::IntRect * leftRectangle = leftChild->insert(inputRectangle);
			if(leftRectangle == NULL)
			{
				return rightChild->insert(inputRectangle);
			}
			return leftRectangle;
		}
		else
		{
			if(filled || inputRectangle->Width > binRectangle->Width || inputRectangle->Height > binRectangle->Height)
			{
				return NULL;
			}

			if(inputRectangle->Width == binRectangle->Width && inputRectangle->Height == binRectangle->Height)
			{
				filled = true;
				return binRectangle;
			}

			int widthDifference = binRectangle->Width - inputRectangle->Width;
			int heightDifference = binRectangle->Height - inputRectangle->Height;

			sf::IntRect * leftRectangle = new sf::IntRect(*binRectangle);
			sf::IntRect * rightRectangle = new sf::IntRect(*binRectangle);

			if(widthDifference > heightDifference)
			{
				leftRectangle->Width = inputRectangle->Width;
				rightRectangle->Left += inputRectangle->Width;
				rightRectangle->Width -= inputRectangle->Width;
			}
			else
			{
				leftRectangle->Height = inputRectangle->Height;
				rightRectangle->Top += inputRectangle->Height;
				rightRectangle->Height -= inputRectangle->Height;
			}

			leftChild = new PackingTreeNode(leftRectangle);
			rightChild = new PackingTreeNode(rightRectangle);

			return leftChild->insert(inputRectangle);
		}
	}
};




#endif