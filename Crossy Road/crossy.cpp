#include <iostream>
#include <stdio.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glut.h>
#include <vector>
#include <cmath>

#include "Sleep.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/TextureManager.h"
#include "ObjLibrary/TextureBmp.h"
#include "GetGlut.h"

#define KEY_ESCAPE 27

using namespace std;

void init();
void initDisplay();
void keyboard(unsigned char key, int x, int y);
void specialInput(int key, int x, int y);
void reshape(int w, int h);
void update();
void display();
void drawQuad(int x, int y);
void player(ObjModel & player);

ObjModel mountain;
ObjModel building;
ObjModel charchater1;
ObjModel car1;

float eyeX = 5.0f, eyeY = 10.0f, eyeZ = 30.0f;
float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f;
float playerX = 0.0f, playerY = 0.0f, playerZ = 20.0f;
float playerSX = 0.09f, playerSY = 0.09f, playerSZ = 0.09f;
float stepZ = 1, stepX = 1;
bool isMovingF = false, isMovingB = false, isMovingL = false, isMovingR = false, isMoving = false;

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);

	glutInitWindowSize(700, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Crossy Road");

	glutSpecialFunc(specialInput);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutIdleFunc(update);
	glutDisplayFunc(display);

	init();

	glutMainLoop();

	return 1;
}

void init()
{
	initDisplay();
	
	mountain.load("mountain/lowpolymountains.obj");
	charchater1.load("chr_old.obj");
	//building.load("Building1.obj");
	//car1.load("car1.obj");

}

void initDisplay()
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.0);
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//GLfloat lmodel_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport (0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLdouble)w / (GLdouble)h, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ,
	          centerX, centerY, centerZ,
	          0.0, 1.0, 0.0);

	//Lighting
	//Light 0
	GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);

	//Material 
	//GLfloat white[] = {0.8f, 0.8f, 0.8f, 1.0f};
	//GLfloat cyan[] = {0.f, .8f, .8f, 1.f};
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	//GLfloat shininess[] = {50};
	//glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

   	glPushMatrix();
        glTranslatef(-20.0f, 0.0f, -12.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glColor3f(0.0f,0.0f,0.0f);
        //glScalef(2.0f, 3.0f, 1.0f);
        mountain.draw();
    glPopMatrix();


	glPushMatrix();
        glTranslatef(-20.0f, 0.0f, 7.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glColor3f(0.0f,0.0f,0.0f);
        //glScalef(2.0f, 3.0f, 1.0f);
        mountain.draw();
    glPopMatrix();

	glPushMatrix();
        glTranslatef(0.0f, 0.0f, 10.0f);
		//glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
		//glColor3f(0.0f,0.0f,0.0f);
        glScalef(0.05f, 0.05f, 0.05f);
        //building.draw();
    glPopMatrix();

	glPushMatrix();
        glTranslatef(5.0f, 0.0f, 20.0f);
		//glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
		//glColor3f(0.0f,0.0f,0.0f);
        glScalef(0.05f, 0.05f, 0.05f);
        //car1.draw();
    glPopMatrix();
	
	player(charchater1);
	
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // on [ESC]
		exit(0); // normal exit
		break;
	}
}

void specialInput(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_UP:
		isMovingF = true;
		isMoving = true;
		break;
	case GLUT_KEY_DOWN:
		isMovingB = true;
		isMoving = true;
		break;
	case GLUT_KEY_LEFT:
		isMovingL = true;
		isMoving = true;
		break;
	case GLUT_KEY_RIGHT:
		isMovingR = true;
		isMoving = true;
		break;
	}

}

void update()
{
	//update your variables here
	sleep(2.0 / 60.0);
	if(!isMoving){
		playerSY = .09;
	}
	if(isMovingF){
		playerZ -= stepZ;
		playerSY = .15;
		isMovingF = false;
		isMoving = false;
	}else if(isMovingB){
		playerZ += stepZ;
		playerSY = .15;
		isMovingB = false;
		isMoving = false;
	}else if(isMovingR){
		playerX += stepX;
		playerSY = .15;
		isMovingR = false;
		isMoving = false;
	}else if(isMovingL){
		playerX -= stepX;
		playerSY = .15;
		isMovingL = false;
		isMoving = false;
	}

	cout << playerX << ":" << playerY << ":" << playerZ << endl;

	//sleep(1.0 / 60.0);
	glutPostRedisplay();
}

void player(ObjModel & player)
{
	glPushMatrix();
        glTranslatef(playerX, playerY, playerZ);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		//glColor3f(0.0f,0.0f,0.0f);
        glScalef(playerSX, playerSY, playerSZ);
        player.draw();
    glPopMatrix();
}
