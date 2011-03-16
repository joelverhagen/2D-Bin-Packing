#include "Rectangle.h"

Rectangle::Rectangle(int left, int top, int right, int bottom)
{
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
}

Rectangle::~Rectangle()
{

}

int Rectangle::getWidth()
{
	return right - left;
}

int Rectangle::getHeight()
{
	return bottom - top;
}

string Rectangle::toString()
{
	stringstream s;
	s << "Rectangle(" << getWidth() << ", " << getHeight() << ")";

	return s.str();
}