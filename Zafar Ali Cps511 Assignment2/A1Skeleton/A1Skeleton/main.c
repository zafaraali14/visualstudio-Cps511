/*******************************************************************
Multi-Part Model Construction and Manipulation
********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <stdbool.h>
#include "Vector3D.h"
#include "QuadMesh.h"
#include "Matrix3D.h"

const GLfloat PI = 3.1415; //PI constant for use in sin and cosine 
const int meshSize = 8;    // Default Mesh Size
const int vWidth = 1200;     // Viewport width in pixels
const int vHeight = 700;    // Viewport height in pixels

//coordinates for entire robot movement
GLfloat xMove = 0.0;
GLfloat yMove = 0.0;
GLfloat zMove = 0.0;

//coordinates for elbow position
GLfloat eMoveX = 1.7;
GLfloat eMoveY = 0.0;
GLfloat eMoveZ = 0.0;

//coordinates for shovel position
GLfloat digMoveX = 1.0;
GLfloat digMoveY = 0.0;
GLfloat digMoveZ = 0.0;

//coordinates for shoulder position
GLfloat sMoveX = 0.0;
GLfloat sMoveY = 0.0;
GLfloat sMoveZ = 0.0;

//lengths of segments 
GLfloat sLength = 3.7;
GLfloat eLength = 2;
GLfloat dLength = 2;


//default angles 
int rAngle = 0;
int shoulder = 75;
int elbow = 80;
int dig = 0;

//used for zoom and mouse control 
double dimension = 10.0;
double eX= 0;
double eY = 10;
double eZ = 15;
double zoom =45.0;

static int currentButton;
static unsigned char currentKey;

//robot isnt moving yet
boolean movingForward = false;
boolean movingBackward = false;
boolean movingRight= false;
boolean movingLeft = false;

// Lighting/shading and material properties for robot

// Light properties
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Material properties
static GLfloat submarine_mat_ambient[] = { 0.4F, 0.2F, 0.0F, 1.0F };
static GLfloat submarine_mat_specular[] = { 0.1F, 0.1F, 0.0F, 1.0F };
static GLfloat submarine_mat_diffuse[] = { 0.9F, 0.5F, 0.0F, 1.0F };
static GLfloat submarine_mat_shininess[] = { 0.0F };

// A quad mesh representing the ground / sea floor 
static QuadMesh groundMesh;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void update();
void movingMouse(int xMouse, int yMouse);

//This function moves the cube to the tip of the shovel
Vector3D tipCalc(GLfloat x, GLfloat y, GLfloat z) {
	
	Matrix3D identity = NewIdentity()  ;
	Vector3D tip = NewVector3D(x,y,z);
	

	MatrixLeftMultiplyV(&identity, NewTranslate(-x, -y,-z));

	//translate to end of shovel
	MatrixLeftMultiplyV(&identity, NewTranslate((dLength-0.2),digMoveY+0.2,digMoveZ));

	//translate to the end of elbow boom
	MatrixLeftMultiplyV(&identity, NewTranslate((eLength), eMoveY, eMoveZ));

	//rotate elbow pitch
	MatrixLeftMultiplyV(&identity, NewRotateZ(elbow));

	//translate to end of shoulder boom
	MatrixLeftMultiplyV(&identity, NewTranslate(sLength, sMoveY, sMoveZ));
	
	//rotation of shoulder pitch
	MatrixLeftMultiplyV(&identity, NewRotateZ(shoulder));

	//rotation of shoulder yaw 
	MatrixLeftMultiplyV(&identity, NewRotateY(-rAngle));
	
	MatrixLeftMultiplyV(&identity, NewTranslate(x, y, z));
	VectorLeftMultiplyV(&tip, identity);
	

	return tip;
}



//this function controls movement using sin and cosine for the rAngle so when u turn the robot it moves correctly too 
void update()
{


	if (!movingForward && movingBackward)
	{
		zMove -= (-sin(((rAngle * PI) / 180)) * 0.1);
		xMove -= (-cos(((rAngle * PI) / 180)) * 0.1);
		movingBackward = false;
	}
	else if (!movingBackward && movingForward)
	{
	
		zMove += (-sin(((rAngle * PI) / 180)) * 0.1);
		xMove += (-cos(((rAngle * PI) / 180)) * 0.1);
		
		movingForward = false;
	}
	else if (!movingLeft && movingRight) {
		zMove += (-cos(((rAngle * PI) / 180)) * 0.1);
		xMove += (sin(((rAngle * PI) / 180)) * 0.1);
		
		movingRight = false;
	}
	else if (!movingRight && movingLeft) {
		
		zMove -= (-cos(((rAngle * PI) / 180)) * 0.1);
		xMove -= (sin(((rAngle * PI) / 180)) * 0.1);
		
		movingLeft = false;
	}
}




int main(int argc, char **argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("Assignment 2 Robot Digger");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);
	glutPassiveMotionFunc(movingMouse);

	// Start event loop, never returns
	glutMainLoop();
	return 0;
	
}


// Set up OpenGL. For viewport and projection setup see reshape(). */
void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);   // This light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

														 // Set up ground/sea floor quad mesh
	
	Vector3D origin = NewVector3D(-8.0f, 0.0f, 8.0f);
	Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
	Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
	groundMesh = NewQuadMesh(meshSize);
	InitMeshQM(&groundMesh, meshSize, origin, 16.0, 16.0, dir1v, dir2v);

	Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
	Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
	Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
	SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);

}

//MAKE SURE TO CHANGE THIS 
void MeshBreak(Vector3D tip, QuadMesh* q) {
	GLfloat a = 0.01;
	GLfloat b = -0.01;
	GLfloat r;
	int vertexnum;
	GLfloat vectorx;
	GLfloat vectory;	
	GLfloat vectorz;
	int thisQuad=0;
	      
	//loop over each vertex in mesh 
	for (int i = 0; i < meshSize; i++) {
		for (int j = 0; j < meshSize; j++) {
			for (vertexnum = 0; vertexnum < 5; vertexnum++) {

				//calculate vector coordinates 
				vectorx = q->quads[thisQuad].vertices[0]->position.x;
				vectorx = vectorx + 1.75;
				vectory = q->quads[thisQuad].vertices[0]->position.y;
				vectorz = q->quads[thisQuad].vertices[0]->position.z;
				Vector3D vector1 = NewVector3D(vectorx,vectory,vectorz);
				Vector3D result;

				//dig into the mesh
				Subtract(&vector1, &tip, &result);
				r = GetLength(&result);
				if (r < 1) {
					GLfloat guassian = b*pow(2.71, (-a*(pow(r, 2))));
					q->quads[thisQuad].vertices[i]->position.y +=guassian ;
				}
			}
			thisQuad++;
		}
	}


}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(eX, eY, eZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	

	// Set robot material properties
	glMaterialfv(GL_FRONT, GL_AMBIENT, submarine_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, submarine_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, submarine_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, submarine_mat_shininess);
	
	//constraints to joint movement
	if (shoulder <= 75) shoulder = 75;
	if (shoulder >= 155) shoulder = 155;
	if (elbow <= 0) elbow = 0;
	if (elbow >= 155) elbow = 155; 
	
	//put everything in one big push pop for robot movement
	glPushMatrix();
	glTranslatef(xMove, 0, zMove);
	
	//draw base
	glPushMatrix();
	glScalef(0.6, 0.6, 0.6);
	glutSolidSphere(2, 8, 8);
	glPopMatrix();

	//put all shoulder links in one big push pop
	glPushMatrix();

	glTranslatef(sMoveX, sMoveY, sMoveZ);
	glRotatef(rAngle, 0, -1, 0);
	glRotatef((GLfloat)shoulder, 0.0, 0.0, 1.0);
	glTranslatef(sLength/2, 0, 0);

	//draw shoulder
	glPushMatrix();
	glScalef(3.7, 0.5, 0.5);
	glutSolidCube(1);
	glPopMatrix();
	
	//put elbow and shovel in one big push pop since they are linked 
	glPushMatrix();
	glTranslatef(eMoveX, eMoveY, eMoveZ);
	glRotatef((GLfloat)elbow, 0.0, 0.0, 1.0);
	glTranslatef(eLength / 2, 0, 0);

	//draw elbow
	glPushMatrix();
	glScalef(2, 0.5, 0.5);
	glutSolidCube(1);
	glPopMatrix();

	//draw shovel
	glPushMatrix();
	glTranslatef(digMoveX, digMoveY, digMoveZ);
	glRotatef((GLfloat)dig, 0.0, 0.0, 1.0);
	glTranslatef(dLength / 2, 0, 0);
	glScalef(2, 0.25, 0.5);
	glutSolidCube(1);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	

	//create vector tip and call the tipCalc
	Vector3D tip = tipCalc(xMove, yMove, zMove);

	//when you get the values from tipCalc so you are at the tip of the shovel use those values
	glPushMatrix();
	glTranslatef(tip.x, tip.y, tip.z);
	
	//draw the cube to see the tip (you can comment this out since its not visibly needed for the robot)
	//glutSolidCube(0.7);
	glPopMatrix();
	MeshBreak(tip, &groundMesh);
	DrawMeshQM(&groundMesh, meshSize); // Draw mesh
	
 
	
	glutSwapBuffers();   // Double buffering, swap buffers
}


// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(zoom, (GLdouble)(w / h), 0.2, 40.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}


// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	//forward movement key command 
	 if (key == 'f' || key == 'F')
	{
		movingForward = true;
	
		update();
	}

	 //left movement key command 
	else if (key == 'L' || key == 'l')
	{
		movingLeft = true;
		update();
	}

	 //right movement key command 
	else if (key == 'R' || key == 'r')
	{
		movingRight = true;
		update();
	}

	//backwards movement key command
	else if (key == 'b' || key == 'B')
	{
		movingBackward = true;
		update();
	}

	 //move elbow key commands
	else if (key == 'e') {
		elbow += 5;
		
	}
	else if (key == 'g')elbow -= 5; 

	//move shoulder key commands
	else if (key == 's') {
		shoulder += 5; 
	}
	else if (key == 'd') {
		shoulder -= 5;
		
	}

	//zoom key commands
	else if (key == 'x'|| key=='X') {
		zoom += 5;
		reshape(vWidth, vHeight);
	}

	else if (key == 'z'||key == 'Z') {
		zoom -= 5;
		reshape(vWidth, vHeight);
	}



	glutPostRedisplay();   // Trigger a window redisplay
}

//This method calculates  the eye coordinates for gllookat
void movingMouse(int xMouse, int yMouse) {
	eX= -2 * dimension*sin(3.14 / 180 * xMouse) * cos(3.14 / 180 * yMouse);
	eY = 2 * dimension*sin(3.14 / 180 * yMouse);
	eZ = 2 * dimension*cos(3.14 / 180 * xMouse) * cos(3.14 / 180 * yMouse);
	glutPostRedisplay();

}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key Press F1 the help gods to rain upon you
	if (key == GLUT_KEY_F1)
	{
		printf("Looks like you need help buddy:\n");
		printf("Moving\n");
		printf("- To Move the robot Forward using the timer press 'F' or 'f'\n");
		printf("- To Move the robot Backward using the timer press 'B' or 'b'\n");
		printf("- To Move the robot Left using the timer press 'L' or 'l'\n");
		printf("- To Move the robot Right using the timer press 'R' or 'r'\n");
		printf("-----------------------------------------------------------------\n");
		printf("Rotating\n");
		printf("- To Rotate the robot Left, press the 'Left Arrow Key'.\n");
		printf("- To Rotate the robot Right, press the 'Right Arrow Key'.\n");
		printf("-----------------------------------------------------------------\n");
		printf("Raise and Lower\n");
		printf("- To Raise and Lower the robot elbow use 'e' and 'g'.\n");
		printf("- To Raise and Lower the robot shoulder use 's' and 'd'.\n");
		printf("-----------------------------------------------------------------\n");
		printf("View\n");
		printf("- To Zoom In press 'z' or 'Z'.\n");
		printf("- To Zoom In press 'x' or 'x'.\n");
		printf("- To Control the Camera Move the Mouse.\n");

		
		
	}

	else if (key == GLUT_KEY_RIGHT) //Right Key
	{
		rAngle += 10;
		rAngle %= 360;
	}

	else if (key == GLUT_KEY_LEFT) //Left Key
	{
		rAngle -= 10;
		rAngle %= 360;
	}

	


	glutPostRedisplay();   // Trigger a window redisplay
}