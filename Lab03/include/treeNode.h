#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <string>
#include <vector>
#include <GL/glut.h>
#include "math/vect3d.h"


// class for the basic tree node
class treeNode
{
public:

	treeNode(Vect3d p, Vect3d up, Vect3d right, double len) {
		pos = p;
		upVec = up;
		rightVec = right;
		leftChild = NULL;
		rightChild = NULL;
		length = len;
	}

	treeNode *leftChild;
	treeNode *rightChild;

	Vect3d pos;
	Vect3d upVec;
	Vect3d rightVec;
	double length;
};