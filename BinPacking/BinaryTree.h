#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <iostream>
using namespace std;

template<typename T>
class BinaryTreeNode
{
public:
	T value;
	BinaryTreeNode * leftChild;
	BinaryTreeNode * rightChild;

	BinaryTreeNode(T value)
	{
		this->value = value;
		leftChild = NULL;
		rightChild = NULL;
	}

	~BinaryTreeNode()
	{
		cout << "Deleting " << this << endl;
		delete leftChild;
		delete rightChild;
	}
};

#endif