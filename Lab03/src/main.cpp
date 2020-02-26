/*********************************/
/* CS 590CGS Lab framework        */
/* (C) Bedrich Benes 2020        */
/* bbenes ~ at ~ purdue.edu      */
/* Press +,- to add/remove points*/
/*       r to randomize          */
/*       s to change rotation    */
/*       c to render curve       */
/*       t to render tangents    */
/*       p to render points      */
/*       s to change rotation    */
/*********************************/

#include <stdio.h>
#include <iostream>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <string>
#include <vector>			//Standard template library class
#include <GL/freeglut.h>


//in house created libraries
#include "math/vect3d.h"    //for vector manipulation
#include "math/triangle.h"  //triangles
//#include "helper.h"         
#include "objGen.h"         //to save OBJ file format for 3D printing
#include "trackball.h"
#include "treeNode.h"
#include "geometry.h"

#pragma warning(disable : 4996)
#pragma comment(lib, "freeglut.lib")

using namespace std;

//some trackball variables -> used in mouse feedback
TrackBallC trackball;
bool mouseLeft, mouseMid, mouseRight;
bool needRedisplay=false;

vector <Vect3d> v;   //all the points will be stored here
vector <TriangleC> tri;   //all the triangles will be stored here

//window size
GLint wWindow=1200;
GLint hWindow=800;

Vect3d red(1, 0, 0), green(0, 1, 0), blue(0, 0, 1), almostBlack(0.1f, 0.1f, 0.1f), yellow(1, 1, 0);

static int maxSubTime = 2;

double treeLenDecRate = 0.8f;
double treeRotAngle = 60.0f;
double treeTwistAngle = 90.0f;
double boxSize = 0.125f;
int treeHeight = 4;
int subdivisionTime = 0;

treeNode* treeRoot;

deque<Vertex *> vertices;
deque<Edge *> edges;
deque<Surface *> surfaces;

treeNode* GenTree(Vect3d pos, Vect3d up, Vect3d right, double len, int height); // generate the tree nodes recursively
void InitTree();

void GenOBJ() {
	tri.clear();
	for (int i = 0; i < surfaces.size(); i++) {
		TriangleC tmp;
		tmp.Set(surfaces[i]->vertices[0]->pos, surfaces[i]->vertices[1]->pos, surfaces[i]->vertices[2]->pos); //store them for 3D printing
		tri.push_back(tmp);
		tmp.Set(surfaces[i]->vertices[0]->pos, surfaces[i]->vertices[2]->pos, surfaces[i]->vertices[3]->pos); //store them for 3D printing
		tri.push_back(tmp);
	}

	SaveOBJ(&tri, "geometry1.obj");
}

/*********************************
Some OpenGL-related functions DO NOT TOUCH
**********************************/
//displays the text message in the GL window
void GLMessage(char *message)
{
	static int i;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.f, 100.f, 0.f, 100.f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3ub(0, 0, 255);
	glRasterPos2i(10, 10);
	for (i = 0; i<(int)strlen(message); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//called when a window is reshaped
void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glEnable(GL_DEPTH_TEST);
	//remember the settings for the camera
	wWindow = w;
	hWindow = h;
}

//Some simple rendering routines using old fixed-pipeline OpenGL
//draws line from a to b with color 
void DrawLine(Vect3d a, Vect3d b, Vect3d color) {

	glColor3fv(color);
	glBegin(GL_LINES);
		glVertex3fv(a);
		glVertex3fv(b);
	glEnd();
}

//draws point at a with color 
void DrawPoint(Vect3d a, Vect3d color) {

	glColor3fv(color);
	glPointSize(5);
	glBegin(GL_POINTS);
	 glVertex3fv(a);
	glEnd();
}

/**********************
LAB related MODIFY
***********************/

void GetBoxDownVertices(Vect3d &p1, Vect3d &p2, Vect3d &p3, Vect3d &p4, treeNode *node) {

	Vect3d forward = Vect3d::Cross(node->upVec, node->rightVec);
	p1 = node->pos + boxSize * node->length * (- node->upVec + node->rightVec + forward);
	p2 = node->pos + boxSize * node->length * (- node->upVec + node->rightVec - forward);
	p3 = node->pos + boxSize * node->length * (- node->upVec - node->rightVec - forward);
	p4 = node->pos + boxSize * node->length * (- node->upVec - node->rightVec + forward);
}

void GetBoxUpVertices(Vect3d &p1, Vect3d &p2, Vect3d &p3, Vect3d &p4, treeNode *node) {

	Vect3d forward = Vect3d::Cross(node->upVec, node->rightVec);
	p1 = node->pos + boxSize * node->length * (node->upVec + node->rightVec + forward);
	p2 = node->pos + boxSize * node->length * (node->upVec + node->rightVec - forward);
	p3 = node->pos + boxSize * node->length * (node->upVec - node->rightVec - forward);
	p4 = node->pos + boxSize * node->length * (node->upVec - node->rightVec + forward);
}

treeNode* GenTree(Vect3d pos, Vect3d up, Vect3d right, double len, int height) {

	if (height > treeHeight) return NULL;
	treeNode *node = new treeNode(pos, up, right, len);

	Vect3d axis = Vect3d::Cross(up, right);
	Vect3d rotatedUp = up.GetRotatedAxis(treeRotAngle, axis);
	Vect3d rotatedRight = right.GetRotatedAxis(treeRotAngle, axis);

	node->leftChild = GenTree(pos + up * len, up, right.GetRotatedAxis(treeTwistAngle, up), len * treeLenDecRate, height + 1);
	node->rightChild = GenTree(pos + rotatedUp * len, rotatedUp, rotatedRight.GetRotatedAxis(treeTwistAngle, rotatedUp), len * treeLenDecRate, height + 1);

	return node;
}

void GenBoxFaces(treeNode *node) {

	if (node == NULL) return;

	Vect3d pos1, pos2, pos3, pos4, pos5, pos6, pos7, pos8;
	GetBoxDownVertices(pos1, pos2, pos3, pos4, node);
	GetBoxUpVertices(pos5, pos6, pos7, pos8, node);

	Vertex *v1 = new Vertex(pos1, 0);
	Vertex *v2 = new Vertex(pos2, 0);
	Vertex *v3 = new Vertex(pos3, 0);
	Vertex *v4 = new Vertex(pos4, 0);
	Vertex *v5 = new Vertex(pos5, 0);
	Vertex *v6 = new Vertex(pos6, 0);
	Vertex *v7 = new Vertex(pos7, 0);
	Vertex *v8 = new Vertex(pos8, 0);

	surfaces.push_back(new Surface(v5, v6, v2, v1, vertices, edges, 0));
	surfaces.push_back(new Surface(v6, v7, v3, v2, vertices, edges, 0));
	surfaces.push_back(new Surface(v7, v8, v4, v3, vertices, edges, 0));
	surfaces.push_back(new Surface(v8, v5, v1, v4, vertices, edges, 0));

	if (node->leftChild != NULL) {
		GetBoxDownVertices(pos1, pos2, pos3, pos4, node->leftChild);
		v1 = new Vertex(pos1, 0);
		v2 = new Vertex(pos2, 0);
		v3 = new Vertex(pos3, 0);
		v4 = new Vertex(pos4, 0);
		surfaces.push_back(new Surface(v5, v6, v2, v1, vertices, edges, 0));
		surfaces.push_back(new Surface(v6, v7, v3, v2, vertices, edges, 0));
		surfaces.push_back(new Surface(v7, v8, v4, v3, vertices, edges, 0));
		surfaces.push_back(new Surface(v8, v5, v1, v4, vertices, edges, 0));
	}
	else {
		Surface *f = new Surface(v5, v6, v7, v8, vertices, edges, 0);
		surfaces.push_back(f);
	}

	if (node->rightChild != NULL) {
		GetBoxDownVertices(pos1, pos2, pos3, pos4, node);
		v1 = new Vertex(pos1, 0);
		v2 = new Vertex(pos2, 0);
		v3 = new Vertex(pos3, 0);
		v4 = new Vertex(pos4, 0);

		GetBoxDownVertices(pos1, pos2, pos3, pos4, node->rightChild);
		v4 = new Vertex(pos1, 0);
		v3 = new Vertex(pos2, 0);
		v7 = new Vertex(pos3, 0);
		v8 = new Vertex(pos4, 0);
		surfaces.push_back(new Surface(v1, v2, v3, v4, vertices, edges, 0));
		surfaces.push_back(new Surface(v2, v6, v7, v3, vertices, edges, 0));
		surfaces.push_back(new Surface(v6, v5, v8, v7, vertices, edges, 0));
		surfaces.push_back(new Surface(v5, v1, v4, v8, vertices, edges, 0));
	}

	GenBoxFaces(node->leftChild);
	GenBoxFaces(node->rightChild);
}

void subdivision(deque<Surface *> &faceQueue, deque<Edge *> &edgeQueue, deque<Vertex *> &vertexQueue, int l)
{
	Surface *f = faceQueue.front();
	int currentLevel = f->level;
	while (f->level < l)
	{
		Vertex *facePoint = new Vertex(f->midPoint, f->level + 1);

		Vertex *edgePoint1 = new Vertex(f->edges[0]->calNewMidPoint(), f->level + 1);
		Vertex *edgePoint2 = new Vertex(f->edges[1]->calNewMidPoint(), f->level + 1);
		Vertex *edgePoint3 = new Vertex(f->edges[2]->calNewMidPoint(), f->level + 1);
		Vertex *edgePoint4 = new Vertex(f->edges[3]->calNewMidPoint(), f->level + 1);

		Vertex *a = new Vertex(f->vertices[0]->calNewVertex(), f->level + 1);
		Vertex *b = new Vertex(f->vertices[1]->calNewVertex(), f->level + 1);
		Vertex *c = new Vertex(f->vertices[2]->calNewVertex(), f->level + 1);
		Vertex *d = new Vertex(f->vertices[3]->calNewVertex(), f->level + 1);

		faceQueue.push_back(new Surface(a, edgePoint1, facePoint, edgePoint4, vertexQueue, edgeQueue, f->level + 1));
		faceQueue.push_back(new Surface(b, edgePoint2, facePoint, edgePoint1, vertexQueue, edgeQueue, f->level + 1));
		faceQueue.push_back(new Surface(c, edgePoint3, facePoint, edgePoint2, vertexQueue, edgeQueue, f->level + 1));
		faceQueue.push_back(new Surface(d, edgePoint4, facePoint, edgePoint3, vertexQueue, edgeQueue, f->level + 1));

		faceQueue.pop_front();
		f = faceQueue.front();

		if (f->level > currentLevel)
		{
			while (vertexQueue.front()->level == currentLevel)
				vertexQueue.pop_front();
			while (edgeQueue.front()->level == currentLevel)
				edgeQueue.pop_front();

			currentLevel = f->level;
		}
	}
}

void InitTree() {

	vertices.clear();
	edges.clear();
	surfaces.clear();

	treeRoot = new treeNode(Vect3d(0.0, -1.0, 0.0), Vect3d(0.0, 1.0, 0.0), Vect3d(1.0, 0.0, 0.0), 0.7);
	treeRoot->rightChild = NULL;
	treeRoot->leftChild = GenTree(treeRoot->pos + treeRoot->upVec * treeRoot->length, treeRoot->upVec, treeRoot->rightVec, treeRoot->length * treeLenDecRate, 1);

	Vect3d pos1, pos2, pos3, pos4;
	GetBoxDownVertices(pos1, pos2, pos3, pos4, treeRoot);

	Vertex *v1 = new Vertex(pos1, 0);
	Vertex *v2 = new Vertex(pos2, 0);
	Vertex *v3 = new Vertex(pos3, 0);
	Vertex *v4 = new Vertex(pos4, 0);

	Surface *f = new Surface(v1, v2, v3, v4, vertices, edges, 0);
	surfaces.push_back(f);

	GenBoxFaces(treeRoot);
	subdivision(surfaces, edges, vertices, subdivisionTime);
}

void RenderTree(treeNode *node) {

	if (node == NULL) return;

	DrawPoint(node->pos, red);
	if (node->leftChild != NULL) {
		DrawLine(node->pos, node->leftChild->pos, green);
		RenderTree(node->leftChild);
	}
	if (node->rightChild != NULL) {
		DrawLine(node->pos, node->rightChild->pos, green);
		RenderTree(node->rightChild);
	}
}

void RenderTreeBox() {

	for (int i = 0; i < vertices.size(); i++) {
		DrawPoint(vertices[i]->pos, red);
	}

	for (int i = 0; i < edges.size(); i++) {
		DrawLine(edges[i]->vertices[0]->pos, edges[i]->vertices[1]->pos, green);
	}
}

//the main rendering function
void RenderObjects()
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	//set camera
	glMatrixMode(GL_MODELVIEW);
	trackball.Set3DViewCamera();

	// RenderTree(treeRoot);
	RenderTreeBox();
}

//Add here if you want to control some global behavior
//see the pointFlag and how is it used
void Kbd(unsigned char a, int x, int y)//keyboard callback
{
	switch (a)
	{
	case 27: 
		exit(0); 
		break;

	case 'q':
		treeTwistAngle -= 5.0f;
		if (treeTwistAngle < 0.0f) treeTwistAngle = 0.0f;
		else InitTree();
		break;

	case 'w':
		treeTwistAngle += 5.0f;
		if (treeTwistAngle > 180.0f) treeTwistAngle = 180.0f;
		else InitTree();
		break;

	case 'a':
		treeRotAngle -= 5.0f;
		if (treeRotAngle < 30.0f) treeRotAngle = 30.0f;
		else InitTree();
		break;

	case 's':
		treeRotAngle += 5.0f;
		if (treeRotAngle > 90.0f) treeRotAngle = 90.0f;
		else InitTree();
		break;

	case 'g':
		GenOBJ();
		break;

	case '+':
		subdivisionTime++;
		if (subdivisionTime > maxSubTime) subdivisionTime = maxSubTime;
		else InitTree();
		break;

	case '-':
		subdivisionTime--;
		if (subdivisionTime < 0) subdivisionTime = 0;
		else InitTree();
		break;
	}
	glutPostRedisplay();
}


/*******************
OpenGL code. Do not touch.
******************/
void Idle(void)
{
  glClearColor(0.5f,0.5f,0.5f,1); //background color
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  GLMessage("Lab 3 - CS 590CGS");
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40,(GLfloat)wWindow/(GLfloat)hWindow,0.01,100); //set the camera
  glMatrixMode(GL_MODELVIEW); //set the scene
  glLoadIdentity();
  gluLookAt(0,10,10,0,0,0,0,1,0); //set where the camera is looking at and from. 
  RenderObjects();
  glutSwapBuffers();  
}

void Display(void)
{

}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		trackball.Set(true, x, y);
		mouseLeft = true;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		trackball.Set(false, x, y);
		mouseLeft = false;
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		trackball.Set(true, x, y);
		mouseMid = true;
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
	{
		trackball.Set(true, x, y);
		mouseMid = false;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		trackball.Set(true, x, y);
		mouseRight = true;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		trackball.Set(true, x, y);
		mouseRight = false;
	}
}

void MouseMotion(int x, int y) {
	if (mouseLeft)  trackball.Rotate(x, y);
	if (mouseMid)   trackball.Translate(x, y);
	if (mouseRight) trackball.Zoom(x, y);
	glutPostRedisplay();
}


int main(int argc, char **argv)
{ 
  glutInitDisplayString("stencil>=2 rgb double depth samples");
  glutInit(&argc, argv);
  glutInitWindowSize(wWindow,hWindow);
  glutInitWindowPosition(500,100);
  glutCreateWindow("Surface of Revolution");
  //GLenum err = glewInit();
  // if (GLEW_OK != err){
  // fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  //}
  glutDisplayFunc(Display);
  glutIdleFunc(Idle);
  glutReshapeFunc(Reshape);
  glutKeyboardFunc(Kbd); //+ and -
  glutSpecialUpFunc(NULL); 
  glutSpecialFunc(NULL);
  glutMouseFunc(Mouse);
  glutMotionFunc(MouseMotion);

  InitTree();

  glutMainLoop();
  return 0;        
}
