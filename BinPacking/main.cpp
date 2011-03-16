#include "SFML/Graphics.hpp"

#include <iostream>
#include <string>

using namespace std;

#include "PackingTreeNode.h"
#include "ResourceFile.h"

void drawRectangles(PackingTreeNode * node, sf::RenderWindow &window)
{
	// preorder recursive function to draw all rectangles

	if(node != NULL)
	{
		if(node->filled)
		{
			sf::Vector2f leftTop((float)node->binRectangle->Left, (float)node->binRectangle->Top);
			sf::Vector2f rightTop((float)(node->binRectangle->Left + node->binRectangle->Width), (float)node->binRectangle->Top);
			sf::Vector2f leftBottom((float)node->binRectangle->Left, (float)(node->binRectangle->Top + node->binRectangle->Height));
			sf::Vector2f rightBottom((float)(node->binRectangle->Left + node->binRectangle->Width), (float)(node->binRectangle->Top + node->binRectangle->Height));

			sf::Shape shape;
			shape.AddPoint(leftTop, node->color);
			shape.AddPoint(rightTop, node->color);
			shape.AddPoint(rightBottom, node->color);
			shape.AddPoint(leftBottom, node->color);

			window.Draw(shape);
		}

		drawRectangles(node->leftChild, window);
		drawRectangles(node->rightChild, window);
	}
}

int main()
{
	// load the icon from the resources file
	ResourceFileReader resources("resources.dat");
	resources.read();

	sf::Image icon;

	char * buffer = resources.getFile("icon.png");
    icon.LoadFromMemory(buffer, resources.getEntryFileSize("icon.png"));

	int screenWidth = 500;
	int screenHeight = 500;
	
	// create the window
	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Bin Packing");

	// set the icon
	window.SetIcon(16, 16, icon.GetPixelsPtr());

	window.SetFramerateLimit(30);

	const sf::Input& input = window.GetInput();

	sf::IntRect * binRectangle = new sf::IntRect(0, 0, screenWidth, screenHeight);
	
	// the root bin for the bin packing algorithm
	PackingTreeNode * packingTreeNode = new PackingTreeNode(binRectangle);

	bool running = true;
	bool clicked = false;

	while(running)
	{
		sf::Event event;
		while(window.GetEvent(event))
		{
			if(event.Type == sf::Event::Closed)
			{
				running = false;
			}
		}

		if(input.IsMouseButtonDown(sf::Mouse::Left))
		{
			sf::IntRect * imageRectangle = new sf::IntRect(0, 0, sf::Randomizer::Random(5, 100), sf::Randomizer::Random(5, 100));

			// add the image to the bin
			sf::IntRect * imageLocation = packingTreeNode->insert(imageRectangle);
		}

		window.Clear();

		drawRectangles(packingTreeNode, window);

		window.Display();
	}

	return 0;
}