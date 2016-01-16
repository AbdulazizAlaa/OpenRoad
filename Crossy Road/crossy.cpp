#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <ctime>
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <GL/freeglut.h>


#include "Sleep.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/TextureManager.h"
#include "ObjLibrary/TextureBmp.h"
//#include "GetGlut.h"

#define KEY_ESCAPE 27

using namespace std;

void init();
void initDisplay();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void specialInput(int key, int x, int y);
void reshape(int w, int h);
void update();
void display();
void player();
void drawCar(float  y, float z, float scale, int i);
void moveCar(float xmax, float step, int i);
void drawRoad(int x);


ObjModel mountain; //mountain model object
ObjModel building; //building model object
ObjModel player1; //first character model object
ObjModel car1; //car model object
ObjModel road;
ObjModel grass;

struct StepObj {
	//ObjModel model;
	bool needUpdate;
	int z;
	int type;
};

StepObj stepObj[10];
float roadZ = -30.0f, roadX = 10.0f;
float carXMin = -20.f;
float count_steps[]={carXMin, carXMin, carXMin, carXMin, carXMin, carXMin, carXMin, carXMin, carXMin, carXMin}; // array of counters for the car steps
float eyeX = 5.0f, eyeY = 10.0f, eyeZ = -10.0f; // Look up eye (camera) position x,y,z
float centerX = -5.0f, centerY = -5.0f, centerZ = -30.0f; // Look up center look position x,y,z
float playerX = 0.0f, playerY = 0.0f, playerZ = -20.0f; // Player Position x,y,z
float playerSX = 0.09f, playerSY = 0.09f, playerSZ = 0.09f; // Player scale x,y,z
float playerAngle = 180.f;
float stepZ = 4, stepX = 1; // Player step in y,z
bool isMovingF = false, isMovingB = false, isMovingL = false, isMovingR = false, isMoving = false; // booleans for moving in every direction
bool isDead = false;
float playerLastZ = playerZ; // Last position of player for camera z position 
float minWX = 0, maxWX = 0; // min and max position player can go in the window
int winW = 500, winH = 700; // width and height of the window

int main(int argc, char* argv[])
{
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB | GLUT_MULTISAMPLE);
	glutInitWindowSize(winH, winW);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Crossy Road");

	glutSpecialFunc(specialInput);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutIdleFunc(update);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);

	init();

	//Initialize step object buffer update bool
	for (int i = 0; i < 10; i++)
	{
		stepObj[i].needUpdate = true;
		stepObj[i].z = -10*i;
		stepObj[i].type = rand() % 2;	
		stepObj[i].needUpdate = false;
	}

	glutMainLoop();

	return 1;
}

void init()
{
	initDisplay();
	
	player1.load("chr_old.obj");
	car1.load("car1.obj");
	road.load("Road_Wide.obj");
	grass.load("Grass.obj");


}

void initDisplay()
{
	glClearColor(0.0f, 0.5f, 1.0f, 0.0);
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
	GLfloat light_position[] = { 10.0, 25.0, 1.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0);

	drawRoad(0);
	drawRoad(1);
	drawRoad(2);
	drawRoad(3);
	drawRoad(4);
	drawRoad(5);
	drawRoad(6);
	drawRoad(7);
	drawRoad(8);
	drawRoad(9);
	
	player();

	/*if(isDead){
		glRasterPos2i(100, 120);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		string s = "sss";

		glutBitmapString(GLUT_BITMAP_HELVETICA_18, s.c_str());
	}*/

	glutSwapBuffers();
}

void player()
{
	glPushMatrix();
        glTranslatef(playerX, playerY, playerZ);
		glRotatef(playerAngle, 0.0f, 1.0f, 0.0f);
        glScalef(playerSX, playerSY, playerSZ);
        player1.draw();
    glPopMatrix();
}

void drawRoad(int i)
{
	glPushMatrix();
		glTranslatef(0, -1.0f, stepObj[i].z);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		if(stepObj[i].type == 0){
			road.draw();
			drawCar(10.0f, 0, 0.09f, i);
		}else if(stepObj[i].type == 1){
			grass.draw();
		}
	glPopMatrix();
		

}

void moveCar(float xmax, float step, int i)
{
	if(count_steps[i] < xmax)
		count_steps[i] += step;
}

void drawCar(float  y, float z, float scale, int i)
{
	glPushMatrix();
		glTranslatef(0, 1, count_steps[i]);
		glScalef(scale,scale,scale);
		//glRotatef(90.0f, 0.0f, 1.0f, 0.0f);        
		car1.draw();
	glPopMatrix();
}

void mouse(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON){
		//cout << x << ":" << y <<endl;
		//cout << winW << ":" << winH << endl;
		cout << abs(playerZ)-abs(playerLastZ) << endl;
		cout << playerX << ":" << playerY << ":" << playerZ << endl;
	}
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
	
	for(int i=0 ; i<10 ; i++)
	{
		
		if(playerZ > stepObj[i].z-2 && playerZ < stepObj[i].z+2){
			//cout << i << "collisionZ ::" << count_steps[i] << endl;
			if(playerX > count_steps[i]-3 && playerX < count_steps[i]+3){
				cout << "car: " <<  i << " :: collision" << endl;
				isDead = true;
			}
		}

	}


	if(!isDead){
	
		if(abs(playerZ)-abs(playerLastZ) >= 1){
			eyeZ -= 4.0f;
			centerZ -= 4.0f;
			playerLastZ = playerZ;
		}
		if(!isMoving){
			playerSY = .09;
		}else{
			PlaySound(L"pingas.wav", NULL, SND_ASYNC|SND_FILENAME);
		}

		if(playerX < -10.0f){
			// do nothing
			playerX = -10.0f;
			playerSY = .09;
		}if(playerX > 9.0f){
			playerX = 9.0f;
			playerSY = .09;
		}else if(isMovingF){
			playerZ -= stepZ;
			playerSY = .15;
			playerAngle = 180;
			isMovingF = false;
			isMoving = false;
		}else if(isMovingB){
			playerZ += stepZ;
			playerSY = .15;
			playerAngle = 0;
			isMovingB = false;
			isMoving = false;
		}else if(isMovingR){
			playerX += stepX;
			playerSY = .15;
			playerAngle = 90;
			isMovingR = false;
			isMoving = false;
		}else if(isMovingL){
			playerX -= stepX;
			playerSY = .15;
			playerAngle = 270;
			isMovingL = false;
			isMoving = false;
		}
	
		for (int i = 0; i < 10; i++)
		{
			if(abs(stepObj[i].z - playerZ) > 20 && stepObj[i].z > playerZ)
			{
				stepObj[i].z -= 100;
				stepObj[i].needUpdate = true;
				count_steps[i] = carXMin;
			}			
			if(stepObj[i].needUpdate)
			{
				stepObj[i].type = rand() % 2;
				stepObj[i].needUpdate = false;
			}
		}

	}else{
		playerSX = .2;
		playerSY = .01;
		playerSZ = .2;
	}

	for(int i=0 ; i<10 ; i++)
	{
		moveCar(100, 0.5f, i);
	}
	
	winW = glutGet(GLUT_WINDOW_WIDTH);
	winH = glutGet(GLUT_WINDOW_HEIGHT);
	//cout << playerX << ":" << playerY << ":" << playerZ << endl;

	//sleep(1.0 / 60.0);
	glutPostRedisplay();
}