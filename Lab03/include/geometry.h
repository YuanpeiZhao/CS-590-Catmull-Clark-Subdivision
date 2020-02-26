#include <stdio.h>
#include <math.h>
#include <vector>
#include <deque>
#include "math/vect3d.h"

using namespace std;

class Vertex;
class Edge;
class Surface;

// class for the basic tree node
class Vertex
{
public:

	Vertex(Vect3d position, int l) {
		pos = position;
		level = l;
	}

	vector<Edge *> edges;
	vector<Surface *> surfaces;

	Vect3d pos;
	int level;

	Vect3d calNewVertex();
	bool operator ==(const Vertex &v);
};

class Edge
{
public:

	Edge(Vertex &v1, Vertex &v2, int l) {
		vertices.push_back(&v1);
		vertices.push_back(&v2);
		v1.edges.push_back(this);
		v2.edges.push_back(this);
		level = l;
		midPoint = (v1.pos + v2.pos) / 2.0f;
	}
	vector<Vertex *> vertices;
	vector<Surface *> surfaces;

	Vect3d midPoint;
	int level;

	Vect3d calNewMidPoint();
};

class Surface
{
public:

	Surface(Vertex *&v1, Vertex *&v2, Vertex *&v3, Vertex *&v4, deque<Vertex *>&vQueue, deque<Edge *>&eQueue, int l) {
		midPoint = ((*v1).pos + (*v2).pos + (*v3).pos + (*v4).pos) / 4.0f;
		level = l;

		addVertex(v1, vQueue);
		addVertex(v2, vQueue);
		addVertex(v3, vQueue);
		addVertex(v4, vQueue);

		addEdge(v1, v2, eQueue);
		addEdge(v2, v3, eQueue);
		addEdge(v3, v4, eQueue);
		addEdge(v4, v1, eQueue);
	}
	vector<Vertex *> vertices;
	vector<Edge *> edges;

	Vect3d midPoint;
	int level;

	void addEdge(Vertex *&v1, Vertex *&v2, deque<Edge *> &eQueue);
	void addVertex(Vertex *&v, deque<Vertex *> &vQueue);
};