#include <fstream> 
#include <vector> 
#include <GL/freeglut.h>
#include <memory.h>
#include <math.h>
#include "math/vect3d.h"
#include "math/triangle.h"
#include "plyGen.h"


using namespace std;

void SavePLY(std::vector <TriangleC> *v, char *filename) {

	ofstream myfile;
	myfile.open(filename);

	myfile << "ply\n";
	myfile << "format ascii 1.0\n";
	myfile << "comment made by Bedrich Benes bbenes@purdue.edu\n";
	myfile << "element vertex " << v->size() * 3 << "\n"; //put the number here
	myfile << "property float x\n";
	myfile << "property float y\n";
	myfile << "property float z\n";
	myfile << "element face " << v->size() << "\n";
	myfile << "property list uint8 int32 vertex_index\n";
	myfile << "end_header\n";
	for (unsigned int i = 0; i < v->size(); i++) {
		myfile << v->at(i).a.GetX() << " " << v->at(i).a.GetY() << " " << v->at(i).a.GetZ() << "\n";
		myfile << v->at(i).b.GetX() << " " << v->at(i).b.GetY() << " " << v->at(i).b.GetZ() << "\n";
		myfile << v->at(i).c.GetX() << " " << v->at(i).c.GetY() << " " << v->at(i).c.GetZ() << "\n";
	}
	int counter = 0;
	for (unsigned int i = 0; i < v->size(); i++) {
		myfile << "3 " << counter++ << " " << counter++ << " " << counter++ << " " << "\n";
	}
	myfile.close();

}



