#include <stdio.h>
#include <math.h>
#include "math/vect3d.h"
#include "geometry.h"

using namespace std;

bool Vertex::operator ==(const Vertex &v)
{
	return (abs(pos.x() - v.pos.x()) < 1e-8 &&
		abs(pos.y() - v.pos.y()) < 1e-8 &&
		abs(pos.z() - v.pos.z()) < 1e-8);
}

Vect3d Vertex::calNewVertex()
{
	int n = edges.size();
	Vect3d facePoint(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < n; i++)
		facePoint += surfaces[i]->midPoint;
	facePoint /= float(n);

	Vect3d edgePoint(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < n; i++)
		edgePoint += edges[i]->midPoint;
	edgePoint /= float(n);

	return (facePoint + 2 * edgePoint + float(n-3) * pos) / float(n);
}

Vect3d Edge::calNewMidPoint()
{
	return (surfaces[0]->midPoint + surfaces[1]->midPoint + midPoint) / 3.0f;
	//return midPoint;
}

void Surface::addVertex(Vertex *&v, deque<Vertex *> &vQueue)
{
	Vertex *vPtr = NULL;
	for (int i = 0; i < vQueue.size(); i++) {
		if (*v == *vQueue[i]) {
			vPtr = vQueue[i];
			break;
		}
	}	

	if (vPtr != NULL) v = vPtr;
	else vQueue.push_back(v);

	vertices.push_back(v);
	v->surfaces.push_back(this);
}

void Surface::addEdge(Vertex *&v1, Vertex *&v2, deque<Edge *> &eQueue)
{

	Edge *ePtr = NULL;
	for (int i = 0; i < eQueue.size(); i++) {
		if ((*v1 == *(*eQueue[i]).vertices[0] && *v2 == *(*eQueue[i]).vertices[1]) ||
			(*v2 == *(*eQueue[i]).vertices[0] && *v1 == *(*eQueue[i]).vertices[1])) {
			ePtr = eQueue[i];
			break;
		}
	}

	if(ePtr == NULL) {
		ePtr = new Edge(*v1, *v2, level);
		eQueue.push_back(ePtr);
	}
	edges.push_back(ePtr);
	ePtr->surfaces.push_back(this);
}