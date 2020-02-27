#include <fstream> 
#include <vector> 
#include <GL/freeglut.h>
#include <memory.h>
#include <math.h>
#include "math/vect3d.h"
#include "math/triangle.h"
#include "objGen.h"


using namespace std;

void SaveOBJ(std::vector <TriangleC> *v, char *filename) {

	ofstream myfile;
	myfile.open(filename);

	myfile << "# Generated by Yuanpei Zhao zhao1023@purdue.edu\n";
	myfile << "# vertices\n";
	for (unsigned int i = 0; i < v->size(); i++) {
		myfile << "v " << v->at(i).a.GetZ() << " " << v->at(i).a.GetY() << " " << v->at(i).a.GetX() << "\n";
		myfile << "v " << v->at(i).b.GetZ() << " " << v->at(i).b.GetY() << " " << v->at(i).b.GetX() << "\n";
		myfile << "v " << v->at(i).c.GetZ() << " " << v->at(i).c.GetY() << " " << v->at(i).c.GetX() << "\n";
	}
	int counter = 1;
	myfile << "# faces\n";
	for (unsigned int i = 0; i < v->size(); i++) {
		myfile << "f " << counter++;
		myfile << " " << counter++;
		myfile << " " << counter++ << " " << "\n";
	}
	myfile.close();

}



