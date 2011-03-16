#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <iostream>
#include <sstream>
using namespace std;

class Rectangle
{
public:
	int left;
	int top;
	int right;
	int bottom;

	Rectangle(int left, int top, int right, int bottom);
	~Rectangle();

	int getWidth();
	int getHeight();

	string toString();
};

#endif