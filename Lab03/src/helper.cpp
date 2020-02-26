#include <GL/freeglut.h>
#include "helper.h"


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
	extern GLint wWindow;
	extern GLint hWindow;

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


void Display(void)
{

}



//display coordinate system
void CoordSyst() {
	Vect3d a, b, c;
	Vect3d origin(0, 0, 0);
	Vect3d red(1, 0, 0), green(0, 1, 0), blue(0, 0, 1), almostBlack(0.1f, 0.1f, 0.1f), yellow(1, 1, 0);

	//draw the coordinate system 
	a.Set(1, 0, 0);
	b.Set(0, 1, 0);
	c.Set(Vect3d::Cross(a, b)); //use cross product to find the last vector
	glLineWidth(4);
	DrawLine(origin, a, red);
	DrawLine(origin, b, green);
	DrawLine(origin, c, blue);
	glLineWidth(1);

}