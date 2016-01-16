#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <ctime>
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
void mouse(int button, int state, int x, int y);
void specialInput(int key, int x, int y);
void reshape(int w, int h);
void update();
void display();
void player(ObjModel & player);
void moveObject(ObjModel x,float  z, float  xmin, float xmax,float scale,float  y,int steps,int i=0);

ObjModel mountain; //mountain model object
ObjModel building; //building model object
ObjModel charchater1; //first character model object
ObjModel car1; //car model object
ObjModel road;
ObjModel grass;

struct StepObj {
	ObjModel model;
	bool needUpdate;
	int z;
	int type;
};

StepObj stepObj[15];

float count_steps[]={0,0,0,0,0,0,0,0,0,0}; // array of counters for the car steps
float eyeX = 5.0f, eyeY = 10.0f, eyeZ = -10.0f; // Look up eye (camera) position x,y,z
float centerX = -5.0f, centerY = -5.0f, centerZ = -30.0f; // Look up center look position x,y,z
float playerX = 0.0f, playerY = 0.0f, playerZ = -20.0f; // Player Position x,y,z
float playerSX = 0.09f, playerSY = 0.09f, playerSZ = 0.09f; // Player scale x,y,z
float stepZ = 1, stepX = 1; // Player step in y,z
bool isMovingF = false, isMovingB = false, isMovingL = false, isMovingR = false, isMoving = false; // booleans for moving in every direction
float playerLastZ = playerZ; // Last position of player for camera z position 
float minWX = 0, maxWX = 0; // min and max position player can go in the window
int winW = 500, winH = 700; // width and height of the window

int main(int argc, char* argv[])
{
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);

	glutInitWindowSize(winH, winW);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Crossy Road");

	glutSpecialFunc(specialInput);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutIdleFunc(update);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);

	//Initialize step object buffer update bool
	for (int i = 0; i < 15; i++)
	{
		stepObj[i].needUpdate = true;
		stepObj[i].z = -10*i;
		if(stepObj[i].needUpdate)
		{
			stepObj[i].type = rand() % 2;	
		}
		if(stepObj[i].type == 0)
		{
			stepObj[i].model = road;
			stepObj[i].needUpdate = false;
		}
		else if(stepObj[i].type == 1)
		{
			stepObj[i].model = grass;
			stepObj[i].needUpdate = false;
		}	
	}

	init();

	glutMainLoop();

	return 1;
}

void init()
{
	initDisplay();
	
	mountain.load("mountain/lowpolymountains.obj");
	charchater1.load("chr_old.obj");
	building.load("Building1.obj");
	car1.load("car1.obj");
	road.load("Road_Wide.obj");
	grass.load("Grass.obj");


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

	for (int i = 0; i < 15; i++)
	{
		glPushMatrix();
			glTranslatef(0, -1, stepObj[i].z-(10));
			glRotatef(90, 0, 1, 0);
			stepObj[i].model.draw();
		glPopMatrix();
	}
	

   	glPushMatrix();
        glTranslatef(-20.0f, 0.0f, -30.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glColor3f(0.0f,0.0f,0.0f);
        //glScalef(2.0f, 3.0f, 1.0f);
        mountain.draw();
    glPopMatrix();


	glPushMatrix();
        glTranslatef(-20.0f, 0.0f, -40.0f);
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
	
	moveObject(car1,-10,1,10,0.09,0,2,0);

	glutSwapBuffers();
}

void player(ObjModel & player)
{
	glPushMatrix();
        glTranslatef(playerX, playerY, playerZ);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        glScalef(playerSX, playerSY, playerSZ);
        player.draw();
    glPopMatrix();
}

void moveObject(ObjModel x, float  z, float  xmin, float xmax, float scale, float  y, int steps, int i)
{
		//i created an array of 10 floats assuming we will create no more than 10 cars per frame 
		// steps is the no. that controls the interpolation of a car 
		// i is a variable of each car which is used to get the proper element in count_steps which belongs to this car
		int step1=(xmax-xmin)/steps;
		count_steps[i]+=step1;

		glPushMatrix();
			glScalef(scale,scale,scale);
			glTranslatef(count_steps[i], 0.0f, z);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);        
			x.draw();
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
	//cout << sqrt(abs((playerZ*playerZ)-(playerLastZ*playerLastZ))) << endl;

	if(abs(playerZ)-abs(playerLastZ) >= 1){
		eyeZ -= 1.0f;
		centerZ -= 1.0f;
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

	for (int i = 0; i < 15; i++)
	{
		if(abs(stepObj[i].z - playerZ) < 45 && stepObj[i].z > playerZ)
			{
				stepObj[i].z -= 150;
			}			
			if(stepObj[i].needUpdate)
			{
				stepObj[i].type = rand() % 2;	
			}
			if(stepObj[i].type == 0)
			{
				stepObj[i].model = road;
				stepObj[i].needUpdate = false;
			}
			else if(stepObj[i].type == 1)
			{
				stepObj[i].model = grass;
				stepObj[i].needUpdate = false;
			}
	}


	
	winW = glutGet(GLUT_WINDOW_WIDTH);
	winH = glutGet(GLUT_WINDOW_HEIGHT);
	//cout << playerX << ":" << playerY << ":" << playerZ << endl;

	//sleep(1.0 / 60.0);
	glutPostRedisplay();
}