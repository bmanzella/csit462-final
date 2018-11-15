// HW1.cpp
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <GL/glut.h>
const int WINDOW_WIDTH = 700;
const int WINDOW_HEIGHT = 500;
float rotateLeftArm = 0.0f;
float rotateRightArm = 0.0f;
float rotateHead = 0.0f;
float xRotate = 0;
float yRotate = 0;
float zRotate = 0;

void init(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(-3.0f, 3.0f, -3.0f, 3.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void leftArm()
{
	glPushMatrix();
	glRotatef(rotateLeftArm, 1.0f, 0.0f, 0.0f);
	glTranslatef(-1.0f, 0.1f, 0.0f);
	glScalef(1.0f, 0.2f, 0.2f);
	glutSolidCube(1.0);
	glPopMatrix();
}

void rightArm()
{
	glPushMatrix();
	glRotatef(rotateRightArm, 1.0f, 0.0f, 0.0f);
	glTranslatef(1.0f, 0.1f, 0.0f);
	glScalef(1.0f, 0.2f, 0.2f);
	glutSolidCube(1.0);
	glPopMatrix();
}

void leftLeg()
{
	glPushMatrix();
	glTranslatef(-0.75f, -0.5f, 0.0f);
	glScalef(0.5f, 0.5f, 0.5f);
	glutSolidCube(1.0);
	glPopMatrix();
}

void rightLeg()
{
	glPushMatrix();
	glTranslatef(0.75f, -0.5f, 0.0f);
	glScalef(0.5f, 0.5f, 0.5f);
	glutSolidCube(1.0);
	glPopMatrix();
}

void body()
{
	glPushMatrix();
	glutSolidCube(1.0);
	glPopMatrix();
}

void head()
{
	glPushMatrix();
	glRotatef(rotateHead, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.75f, 0.0f);
	glScalef(1.5f, 0.5f, 1.0f);
	glutSolidCube(1.0);
	glScalef(1.0f, 2.0f, 1.0f);
	glPopMatrix();
}

void resize(int x, int y)
{
	double aspect = WINDOW_WIDTH / WINDOW_HEIGHT;

	if (aspect > (double)x / y)
		glViewport(0, 0, x, x / aspect);
	else
		glViewport(0, 0, y*aspect, y);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);


	glLoadIdentity();

	glTranslatef(0.5f, 0.5f, 0.0f);
	glRotatef(xRotate, 1.0f, 0.0f, 0.0f);
	glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
	glRotatef(zRotate, 0.0f, 0.0f, 1.0f);
	glScalef(1.0f, 2.0f, 1.0f);

	head();
	body();
	leftArm();
	rightArm();
	leftLeg();
	rightLeg();
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	int mode = GLUT_RGBA | GLUT_DOUBLE;

	glutInit(&argc, argv);
	glutInitDisplayMode(mode);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CSIT462 Computer Graphics Final Project");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutMainLoop();
}