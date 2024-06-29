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
#pragma warning(disable:4996)




const GLfloat PI = 3.1415; //PI constant for use in sin and cosine 
const int meshSize = 8;    // Default Mesh Size
const int vWidth = 1200;     // Viewport width in pixels
const int vHeight = 700;    // Viewport height in pixels

//speed variables
int speed = 150;
double speed2 = 0.06;
int backwardSpeed = 150;

boolean bCollide = false;
boolean tCollide = false;
boolean winner = false;

//coordinates for entire robot movement
GLfloat xMove = 5.0;
GLfloat yMove = 0;
GLfloat zMove = 5.0;

GLfloat xMove2 = 0.0;
GLfloat yMove2 = 0;
GLfloat zMove2 = 0.0;
//coordinates for elbow position
GLfloat eMoveX = 1.0;
GLfloat eMoveY = 0;
GLfloat eMoveZ = 0.0;

GLfloat eMoveX2 = 1.0;
GLfloat eMoveY2 = 0;
GLfloat eMoveZ2 = 0.0;

boolean fpvon = false;
//coordinates for shovel position
GLfloat digMoveX = 0.5;
GLfloat digMoveY = 0.0;
GLfloat digMoveZ = 0.0;

GLfloat digMoveX2 = 0.5;
GLfloat digMoveY2 = 0.0;
GLfloat digMoveZ2 = 0.0;

//coordinates for shoulder position
GLfloat sMoveX = 0.0;
GLfloat sMoveY = 0.0;
GLfloat sMoveZ = 0.0;

GLfloat sMoveX2 = 0.0;
GLfloat sMoveY2 = 0.0;
GLfloat sMoveZ2 = 0.0;

//lengths of segments 
GLfloat sLength = 2.4;
GLfloat eLength = 1;
GLfloat dLength = 2;

GLfloat sLength2 = 2.4;
GLfloat eLength2 = 1;
GLfloat dLength2 = 2;

//default angles 
int rAngle = 0;
int shoulder = 80;
int elbow = 80;


int rAngle2 = 180;
int shoulder2 = 100;
int elbow2 = 120;

//used for zoom and mouse control 
double dimension = 10.0;
double eX = 0;
double eY = 10;
double eZ = 15;
double zoom = 60.0;

int health1 = 100;
int health2 = 100;

static int currentButton;
static unsigned char currentKey;

//robot isnt moving yet
boolean movingForward = false;
boolean movingBackward = false;
boolean movingForward2 = false;

boolean movingRight = false;
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
void timerUpdate();
void updateSpeed();
void timerUpdate2();


//move ai robot moving in rangle2 direction
void timerUpdate2()
{

	if (speed2 <= 0) {
		speed2 = 0;
	}
	if (movingForward2)
	{
		
		zMove2 -= (sin(((rAngle2 * PI) / 180)) * speed2);
		xMove2 -= (cos(((rAngle2 * PI) / 180)) *speed2);
		
	}
	if (bCollide) {
		if (movingForward2 )
		{

			zMove2 += (sin(((rAngle2 * PI) / 180)) * 3);
			xMove2 += (cos(((rAngle2 * PI) / 180)) *2);

		}
		bCollide = false;
		
	}
	if (tCollide) {
		if (movingForward2 )
		{

			zMove2 += (sin(((rAngle2 * PI) / 180)) * 3);
			xMove2 += (cos(((rAngle2 * PI) / 180)) * 2);

		}
	
		tCollide = false;
	}
	
	glutPostRedisplay();


}


//move users robot (timer function)
void timerUpdate()
{
	if (movingForward && !movingBackward)
	{

		zMove -= (sin(((rAngle * PI) / 180)) * 0.1);
		xMove -= (cos(((rAngle * PI) / 180)) * 0.1);

	}

	else if (!movingForward && movingBackward)
	{

		zMove += (sin(((rAngle * PI) / 180)) * 0.1);
		xMove += (cos(((rAngle * PI) / 180)) * 0.1);
	}

	glutPostRedisplay();

	if (movingForward)		glutTimerFunc(speed, timerUpdate, 0);
	else if (movingBackward)	glutTimerFunc(backwardSpeed, timerUpdate, 0);
}

void updateSpeed()
{
	if (speed == 150)
	{
		movingForward = false;
		movingBackward = false;
	}

	else if (speed < 150)
	{
		movingForward = true;
		glutTimerFunc(speed, timerUpdate, 0);
	}

	else if (speed > 150)
	{
		if (speed == 250) backwardSpeed = 50;
		else if (speed == 200) backwardSpeed = 100;
		movingBackward = true;
		glutTimerFunc(backwardSpeed, timerUpdate, 0);
	}

}

//This function moves the cube to the tip of the shovel
Vector3D tipCalc(GLfloat x, GLfloat y, GLfloat z) {

	Matrix3D identity = NewIdentity();
	Vector3D tip = NewVector3D(x, y, z);


	MatrixLeftMultiplyV(&identity, NewTranslate(-x, -y, -z));
	
	//translate to end of shovel
	MatrixLeftMultiplyV(&identity, NewTranslate((dLength), digMoveY, digMoveZ));
	

	//translate to the end of elbow boom
	MatrixLeftMultiplyV(&identity, NewTranslate((eLength), eMoveY, eMoveZ));

	//rotate elbow pitch
	MatrixLeftMultiplyV(&identity, NewRotateZ(elbow));

	//translate to end of shoulder boom
	MatrixLeftMultiplyV(&identity, NewTranslate(sLength - .2, sMoveY, sMoveZ));

	//rotation of shoulder pitch
	MatrixLeftMultiplyV(&identity, NewRotateZ(shoulder));

	//rotation of shoulder yaw 
	MatrixLeftMultiplyV(&identity, NewRotateY(-rAngle));

	MatrixLeftMultiplyV(&identity, NewTranslate(x, y, z));
	VectorLeftMultiplyV(&tip, identity);


	return tip;
}

//This function moves the cube to the tip of the shovel
Vector3D tipCalc2(GLfloat x, GLfloat y, GLfloat z) {

	Matrix3D identity = NewIdentity();
	Vector3D tip2 = NewVector3D(x, y, z);


	MatrixLeftMultiplyV(&identity, NewTranslate(-x, -y, -z));

	//translate to end of shovel
	MatrixLeftMultiplyV(&identity, NewTranslate((dLength2), digMoveY2, digMoveZ2));
	
	//translate to the end of elbow boom
	MatrixLeftMultiplyV(&identity, NewTranslate((eLength2), eMoveY2, eMoveZ2));

	//rotate elbow pitch
	MatrixLeftMultiplyV(&identity, NewRotateZ(elbow2));

	//translate to end of shoulder boom
	MatrixLeftMultiplyV(&identity, NewTranslate(sLength2 - .2, sMoveY2, sMoveZ2));

	//rotation of shoulder pitch
	MatrixLeftMultiplyV(&identity, NewRotateZ(shoulder2));

	//rotation of shoulder yaw 
	MatrixLeftMultiplyV(&identity, NewRotateY(-rAngle2));
	
	MatrixLeftMultiplyV(&identity, NewTranslate(x, y, z));
	VectorLeftMultiplyV(&tip2, identity);


	return tip2;
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



typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;
typedef unsigned short ushort;
typedef unsigned long  ulong;

typedef struct RGB
{
	byte r, g, b;
} RGB;

typedef struct RGBpixmap
{
	int nRows, nCols;
	RGB *pixel;
} RGBpixmap;


RGBpixmap pix[8]; // make empty pixmaps

float xSpeed = 0.01f, ySpeed = 0.010f, xAngle = 0, yAngle = 0;


void makeCheckerboard(RGBpixmap *p)
{
	long count = 0;
	int i, j, c;

	p->nRows = p->nCols = 64;
	p->pixel = (RGB *)malloc(3 * p->nRows * p->nCols);

	for (i = 0; i < p->nRows; i++)
		for (j = 0; j < p->nCols; j++)
		{
			c = (((i / 8) + (j / 8)) % 2) * 255;
			p->pixel[count].r = c;
			p->pixel[count].g = c;
			p->pixel[count++].b = 0;
		}
}



/**************************************************************************
*  fskip                                                                 *
*     Skips bytes in a file.                                             *
**************************************************************************/

void fskip(FILE *fp, int num_bytes)
{
	int i;
	for (i = 0; i < num_bytes; i++)
		fgetc(fp);
}


/**************************************************************************
*                                                                        *
*    Loads a bitmap file into memory.                                    *
**************************************************************************/

ushort getShort(FILE *fp) //helper function
{ //BMP format uses little-endian integer types
  // get a 2-byte integer stored in little-endian form
	char ic;
	ushort ip;
	ic = fgetc(fp); ip = ic;  //first byte is little one 
	ic = fgetc(fp);  ip |= ((ushort)ic << 8); // or in high order byte
	return ip;
}
//<<<<<<<<<<<<<<<<<<<< getLong >>>>>>>>>>>>>>>>>>>
ulong getLong(FILE *fp) //helper function
{  //BMP format uses little-endian integer types
   // get a 4-byte integer stored in little-endian form
	ulong ip = 0;
	char ic = 0;
	unsigned char uc = ic;
	ic = fgetc(fp); uc = ic; ip = uc;
	ic = fgetc(fp); uc = ic; ip |= ((ulong)uc << 8);
	ic = fgetc(fp); uc = ic; ip |= ((ulong)uc << 16);
	ic = fgetc(fp); uc = ic; ip |= ((ulong)uc << 24);
	return ip;
}


void readBMPFile(RGBpixmap *pm, char *file)
{
	FILE *fp;
	int numPadBytes, nBytesInRow;
	ulong fileSize;
	ushort reserved1;    // always 0
	ushort reserved2;     // always 0 
	ulong offBits; // offset to image - unreliable
	ulong headerSize;     // always 40
	ulong numCols; // number of columns in image
	ulong numRows; // number of rows in image
	ushort planes;      // always 1 
	ushort bitsPerPixel;    //8 or 24; allow 24 here
	ulong compression;      // must be 0 for uncompressed 
	ulong imageSize;       // total bytes in image 
	ulong xPels;    // always 0 
	ulong yPels;    // always 0 
	ulong numLUTentries;    // 256 for 8 bit, otherwise 0 
	ulong impColors;       // always 0 
	long count;
	char dum;

	/* open the file */
	if ((fp = fopen(file, "rb")) == NULL)
	{
		printf("Error opening file %s.\n", file);
		exit(1);
	}

	/* check to see if it is a valid bitmap file */
	if (fgetc(fp) != 'B' || fgetc(fp) != 'M')
	{
		fclose(fp);
		printf("%s is not a bitmap file.\n", file);
		exit(1);
	}

	fileSize = getLong(fp);
	reserved1 = getShort(fp);    // always 0
	reserved2 = getShort(fp);     // always 0 
	offBits = getLong(fp); // offset to image - unreliable
	headerSize = getLong(fp);     // always 40
	numCols = getLong(fp); // number of columns in image
	numRows = getLong(fp); // number of rows in image
	planes = getShort(fp);      // always 1 
	bitsPerPixel = getShort(fp);    //8 or 24; allow 24 here
	compression = getLong(fp);      // must be 0 for uncompressed 
	imageSize = getLong(fp);       // total bytes in image 
	xPels = getLong(fp);    // always 0 
	yPels = getLong(fp);    // always 0 
	numLUTentries = getLong(fp);    // 256 for 8 bit, otherwise 0 
	impColors = getLong(fp);       // always 0 

	if (bitsPerPixel != 24)
	{ // error - must be a 24 bit uncompressed image
		printf("Error bitsperpixel not 24\n");
		exit(1);
	}
	//add bytes at end of each row so total # is a multiple of 4
	// round up 3*numCols to next mult. of 4
	nBytesInRow = ((3 * numCols + 3) / 4) * 4;
	numPadBytes = nBytesInRow - 3 * numCols; // need this many
	pm->nRows = numRows; // set class's data members
	pm->nCols = numCols;
	pm->pixel = (RGB *)malloc(3 * numRows * numCols);//make space for array
	if (!pm->pixel) return; // out of memory!
	count = 0;
	dum;
	for (ulong row = 0; row < numRows; row++) // read pixel values
	{
		for (ulong col = 0; col < numCols; col++)
		{
			int r, g, b;
			b = fgetc(fp); g = fgetc(fp); r = fgetc(fp); //read bytes
			pm->pixel[count].r = r; //place them in colors
			pm->pixel[count].g = g;
			pm->pixel[count++].b = b;
		}
		fskip(fp, numPadBytes);
	}
	fclose(fp);
}


void setTexture(RGBpixmap *p, GLuint textureID)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, p->nCols, p->nRows, 0, GL_RGB,
		GL_UNSIGNED_BYTE, p->pixel);

}

//read images 
void myInit(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	readBMPFile(&pix[1], "41.bmp"); 
	readBMPFile(&pix[2], "grass.bmp");
	readBMPFile(&pix[3], "plank01.bmp");
	readBMPFile(&pix[4], "metal.bmp");
	readBMPFile(&pix[0], "wheel.bmp");
	readBMPFile(&pix[5], "burnt.bmp");
	readBMPFile(&pix[6], "fire.bmp");
	glViewport(0, 0, 640, 480);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 640.0 / 480.0, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -4.0); // move camera back from default position
	glShadeModel(GL_SMOOTH);

								  // Set up texture mapping assuming no lighting/shading 
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}


void healthUpdate(void) {
	printf("Your Health: %d\n", health1);
	printf("The Computer's Health: %d\n", health2);
	printf("\n");
}


//checks base collision
void baseCollide() {

	if (!bCollide) {
		if ((fabs(zMove2 - zMove)) <= 1 && (fabs(xMove2 - xMove)) <= 1 && (fabs(yMove2 - yMove)) <= 1) {
			
			speed = 150;
			movingForward = false;

			movingBackward = false;
			bCollide = true;
		
			timerUpdate2();
			double difference1 = rand() % 20 + 1;
			double difference2 = rand() % 20 + 1;
			speed2 -= (difference2*0.0006);
			health1 -= difference1;
			health2 -= difference2;
			
			if (!winner)		healthUpdate();
		}
		

	}
	}

//moves ai robot according to boundaries so it doesn't go too far off mesh
void robot2Move()
{

	movingForward2 = true;


	timerUpdate2();
	if (xMove2 >= 10) {
		
		rAngle2 += rand() % (180 + 1 - 90) + 90;

		timerUpdate2();

	}
	if (xMove2 <= -10) {
		
		rAngle2 += rand() % (180 + 1 - 90) + 90;

		timerUpdate2();

	}
	if (zMove2 <= -10) {
		
		rAngle2 += rand() % (180 + 1 - 90) + 90;

		timerUpdate2();

	}
	if (zMove2 >= 10) {
		
		rAngle2 += rand() % (180 + 1 - 90) + 90;

		timerUpdate2();

	}
	baseCollide();

	
}

//check collision user tip with ai base
void tipCollide1(GLfloat tip1x, GLfloat tip1y, GLfloat tip1z, GLfloat tip2x, GLfloat tip2y, GLfloat tip2z) {
	if (!tCollide) {
		if ((fabs(tip1x - xMove2)) <= 0.6 && fabs(tip1y - (yMove2 + 0.75)) <= 0.6 && fabs(tip1z - zMove2) <= 0.6) {
			
			tCollide = true;
			
			
			double difference2 = rand() % 15 + 1;
			health2 -= difference2;
			speed2 -= (difference2*0.0006);
			robot2Move();
			if (!winner) {
				healthUpdate();
				rAngle2 += 90;
				speed = 150;
				movingForward = false;
				movingBackward = false;
			}

		}
	}
}


//check collision ai tip with user base
void tipCollide2(GLfloat tip1x, GLfloat tip1y, GLfloat tip1z, GLfloat tip2x, GLfloat tip2y, GLfloat tip2z) {
	if ((fabs(tip2x - xMove)) <= 0.6 && fabs(tip2y - (yMove + 0.75)) <= 0.6 && fabs(tip2z - zMove) <= 0.6) {

		
		health1 -= rand() % 15 + 1;
		if (!winner) {
			rAngle2 += 90;
			speed = 150;
			movingForward = false;
			movingBackward = false;
			healthUpdate();
		}

	}
}

int main(int argc, char **argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("Assignment 3 Robot Battle");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);
	glutIdleFunc(robot2Move);
	//glutIdleFunc(baseCollide);
	glutPassiveMotionFunc(movingMouse);
	myInit();
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
	glEnable(GL_LIGHT1);   // This light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

														 // Set up ground/sea floor quad mesh

	Vector3D origin = NewVector3D(-12.0f, 0.0f, 12.0f);
	Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
	Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
	groundMesh = NewQuadMesh(meshSize);
	InitMeshQM(&groundMesh, meshSize, origin, 24.0, 24.0, dir1v, dir2v);

	Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
	Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
	Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
	SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);

}

//this function checks the closest mesh y to the robot and makes that the robots y (so it goes in holes and over hills)
void changeYai(QuadMesh* q) {

	int currentQuad = 0;
	GLfloat height;
	GLfloat x;
	GLfloat y;
	GLfloat z;

	GLfloat smallestR;
	Vector3D base = NewVector3D(xMove2, yMove2, zMove2);
	Vector3D vec1;
	Vector3D result;
	GLfloat closeY;
	for (int j = 0; j< meshSize; j++)
	{
		for (int k = 0; k< meshSize; k++)
		{
			for (int vertexnum = 0; vertexnum < 4; vertexnum++) {


				GLfloat x = q->quads[currentQuad].vertices[vertexnum]->position.x;
				GLfloat y = q->quads[currentQuad].vertices[vertexnum]->position.y;
				GLfloat z = q->quads[currentQuad].vertices[vertexnum]->position.z;

				vec1 = NewVector3D(x, y, z);
				Subtract(&base, &vec1, &result);
				GLfloat r = GetLength(&result);

				if (currentQuad == 0) {
					smallestR = r;
					closeY = y;
				}

				if (r<smallestR) {

					smallestR = r;
					closeY = y;
					
				}


			}



			currentQuad++;
		}

	}
	
	yMove2 = closeY;

}

//break the mesh to make holes and hills
void MeshBreak(Vector3D tip, QuadMesh* q,double b) {
	
	int currentQuad = 0;
	GLfloat height;
	GLfloat x;
	GLfloat y;
	GLfloat z;

	GLfloat smallestR;
	Vector3D base = NewVector3D(xMove, yMove, zMove);
	Vector3D vec1;
	Vector3D result;
	GLfloat closeY;
	for (int j = 0; j< meshSize; j++)
	{
		for (int k = 0; k< meshSize; k++)
		{
			for (int vertexnum = 0; vertexnum < 4; vertexnum++) {


				GLfloat x = q->quads[currentQuad].vertices[vertexnum]->position.x;
				GLfloat y = q->quads[currentQuad].vertices[vertexnum]->position.y;
				GLfloat z = q->quads[currentQuad].vertices[vertexnum]->position.z;
				
				Vector3D distance = NewVector3D((x - tip.x), (y - tip.y), (z - tip.z));;
				
				if (GetLength(&distance) < 2)
				{

					
					height=b* (pow(2.7, (-0.1 * (pow(GetLength(&distance), 2)))));
					q->quads[currentQuad].vertices[vertexnum]->position.y += height;
					q->quads[currentQuad].vertices[vertexnum]->normal.y += height;
				}
				vec1 = NewVector3D(x, y, z);
				Subtract(&base, &vec1, &result);
				GLfloat r = GetLength(&result);

				if (currentQuad == 0) {
					smallestR = r;
					closeY = y;
				}

				if (r<smallestR) {

					smallestR = r;
					closeY = y;
					
				}


			}



			currentQuad++;
		}

	}
	
	yMove = closeY;

}

//change gluLookAt so that it is where rAngle points to
void fpv(void) {




		GLfloat zfpv = (-sin(((rAngle * PI) / 180)) * 200);
GLfloat xfpv = (-cos(((rAngle * PI) / 180)) * 200);

	gluLookAt(xMove, yMove+4.75,zMove, xfpv, yMove+1, zfpv, 0.0, 1.0, 0.0);
	
	
	
}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	if (!fpvon) {
		gluLookAt(eX, eY, eZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}
	else {
		fpv();
	}




	// Set robot material properties
	glMaterialfv(GL_FRONT, GL_AMBIENT, submarine_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, submarine_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, submarine_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, submarine_mat_shininess);

	glBindTexture(GL_TEXTURE_2D, 0);
	setTexture(&pix[2], 2006);
	glPushMatrix();
	
	DrawMeshQM(&groundMesh, meshSize); // Draw mesh
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);

	//create vector tip and call the tipCalc
	Vector3D tip = tipCalc(xMove, yMove+0.75, zMove);





	//when you get the values from tipCalc so you are at the tip of the shovel use those values
	glPushMatrix();
	glTranslatef(tip.x, tip.y, tip.z);

	//draw the cube to see the tip (you can comment this out since its not visibly needed for the robot)
	//glutSolidCube(0.5);
	glPopMatrix(); 
	//create vector tip and call the tipCalc
	Vector3D tip2 = tipCalc2(xMove2, yMove2+0.75, zMove2);
	Vector3D tip3 = NewVector3D(2.510124f, 0.263266f, 0.000000f);
	Vector3D tip4 = NewVector3D(0.010298f, 0.445848f, 9.134580f);
	Vector3D tip5 = NewVector3D(-6.436615f, 0.106505f, 0.000000f);
	Vector3D tip6 = NewVector3D(-6.469237f, 0.196674f, -9.060553f);



	

	//when you get the values from tipCalc so you are at the tip of the shovel use those values
	glPushMatrix();
	glTranslatef(tip2.x, tip2.y, tip2.z);

	//draw the cube to see the tip (you can comment this out since its not visibly needed for the robot)
	//glutSolidCube(0.5);

	//check tip collision
	tipCollide1(tip.x, tip.y, tip.z, tip2.x, tip2.y, tip2.z);
	tipCollide2(tip.x, tip.y, tip.z, tip2.x, tip2.y, tip2.z);
	glPopMatrix();
	
	//break the mesh at specific points of tip3-5 to make hard coded hills and holes 
	MeshBreak(tip3, &groundMesh, -2);
	MeshBreak(tip4, &groundMesh, -2);
	MeshBreak(tip5, &groundMesh, 0.6);
	
	changeYai(&groundMesh);

	//constraints to joint movement
	if (shoulder <= 75) shoulder = 75;
	if (shoulder >= 155) shoulder = 155;
	if (elbow <= 0) elbow = 0;
	if (elbow >= 155) elbow = 155;

	//collision for mesh ending
	if (xMove < -10)xMove = -10;
	if (xMove > 10) xMove = 10;
	if (zMove < -10) zMove = -10;
	if (zMove > 10) zMove = 10;

	
	//declare winner and make sure healths dont go negative
	if (health1 <= 0) {
		health1= 0;
		
		printf("Computer Won\n");
		winner = true;
	}
	if (health2 <= 0) {
		health2 = 0;
		speed2 = 0;
		printf("You Won\n");
		winner = true;
	}
	
	//put everything in one big push pop for robot movement
	glPushMatrix();
	
	glTranslatef(xMove, yMove+0.75, zMove);

	//draw base
	glPushMatrix();
	if (health1 <=100 && health1>90) {
		setTexture(&pix[1], 2005);
	}
	else if (health1<=90 &&health1>50) {
		setTexture(&pix[5], 2008);
	}
	else if (health1<=50){
		setTexture(&pix[6], 2010);
	}
	
	glBegin(GL_QUADS);
	glBegin(GL_QUADS);

	// top
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f, 0.5f, -0.5f);

	glEnd();





	glBegin(GL_QUADS);
	// front
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	glEnd();

	glBegin(GL_QUADS);
	// right
	glColor3f(0.0f, 0.0f, 1.0f);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f, -0.5f, -0.5f);

	glEnd();

	glBegin(GL_QUADS);
	// left
	glColor3f(0.0f, 0.0f, 0.5f);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	glEnd();

	glBegin(GL_QUADS);
	// bottom
	glColor3f(0.5f, 0.0f, 0.0f);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f, -0.5f, -0.5f);

	glEnd();

	glBegin(GL_QUADS);
	// back
	glColor3f(0.0f, 0.5f, 0.0f);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f, 0.5f, -0.5f);

	glEnd();
	glPopMatrix();

	//put all shoulder links in one big push pop
	glPushMatrix();

	glTranslatef(sMoveX, sMoveY, sMoveZ);
	glRotatef(rAngle, 0, -1, 0);
	glRotatef((GLfloat)shoulder, 0.0, 0.0, 1.0);
	glTranslatef(sLength / 2, 0, 0);

	//draw shoulder
	glPushMatrix();
	
	
	glBegin(GL_QUADS);
	// top
	
	glNormal3f(0.0f*sLength, 1.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f*sLength, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*sLength, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f*sLength, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*sLength, 0.5f*0.5, -0.5f*0.5);

	glEnd();

	
	glBegin(GL_QUADS);
	// front
	glColor3f(0.0f*sLength, 1.0f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*sLength, 0.0f*0.5, 1.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*sLength, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*sLength, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*sLength, 0.5f*0.5, 0.5f*0.5);
		glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*sLength, -0.5f*0.5, 0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// right
	glColor3f(0.0f*sLength, 0.0f*0.5, 1.0f*0.5);
	glNormal3f(1.0f*sLength, 0.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*sLength, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*sLength, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f*sLength, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f*sLength, -0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// left
	glColor3f(0.0f*sLength, 0.0f*0.5, 0.5f*0.5);
	glNormal3f(-1.0f*sLength, 0.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f*sLength, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f*sLength, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*sLength, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*sLength, -0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// bottom
	glColor3f(0.5f*sLength, 0.0f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*sLength, -1.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*sLength, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f*sLength, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*sLength, -0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f*sLength, -0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// back
	glColor3f(0.0f*sLength, 0.5f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*sLength, 0.0f*0.5, -1.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*sLength, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*sLength, -0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*sLength, -0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*sLength, 0.5f*0.5, -0.5f*0.5);

	glEnd();
	glPopMatrix();

	//put elbow and shovel in one big push pop since they are linked 
	glPushMatrix();
	glTranslatef(eMoveX, eMoveY, eMoveZ);
	glRotatef((GLfloat)elbow, 0.0, 0.0, 1.0);
	glTranslatef(eLength / 2, 0, 0);
	
	//draw elbow
	glPushMatrix();
	glBegin(GL_QUADS);
	// top
	glColor3f(1.0f*eLength, 0.0f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*eLength, 1.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f*eLength, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*eLength, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f*eLength, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*eLength, 0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// front
	glColor3f(0.0f*eLength, 1.0f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*eLength, 0.0f*0.5, 1.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*eLength, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*eLength, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*eLength, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*eLength, -0.5f*0.5, 0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// right
	glColor3f(0.0f*eLength, 0.0f*0.5, 1.0f*0.5);
	glNormal3f(1.0f*eLength, 0.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*eLength, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*eLength, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f*eLength, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f*eLength, -0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// left
	
	glNormal3f(-1.0f*eLength, 0.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f*eLength, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f*eLength, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*eLength, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*eLength, -0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// bottom
	glColor3f(0.5f*eLength, 0.0f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*eLength, -1.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*eLength, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f*eLength, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*eLength, -0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f*eLength, -0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// back
	glColor3f(0.0f*eLength, 0.5f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*eLength, 0.0f*0.5, -1.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*eLength, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*eLength, -0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*eLength, -0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*eLength, 0.5f*0.5, -0.5f*0.5);

	glEnd();

	glPopMatrix();

	//draw shovel
	
	glPushMatrix();
	glTranslatef(digMoveX, digMoveY, digMoveZ);
	
	glTranslatef(dLength / 2, 0, 0);
	setTexture(&pix[4], 2007);
	glBegin(GL_QUADS);
	// top
	glColor3f(1.0f*dLength, 0.0f*0.25, 0.0f*0.5);
	glNormal3f(0.0f*dLength, 1.0f*0.25, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f*dLength, 0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*dLength, 0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f*dLength, 0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*dLength, 0.5f*0.25, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// front
	glColor3f(0.0f*dLength, 1.0f*0.25, 0.0f*0.5);
	glNormal3f(0.0f*dLength, 0.0f*0.25, 1.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*dLength, -0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*dLength, 0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*dLength, 0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*dLength, -0.5f*0.25, 0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// right
	glColor3f(0.0f*dLength, 0.0f*0.25, 1.0f*0.5);
	glNormal3f(1.0f*dLength, 0.0f*0.25, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*dLength, 0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*dLength, 0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f*dLength, -0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f*dLength, -0.5f*0.25, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// left
	glColor3f(0.0f*dLength, 0.0f*0.25, 0.5f*0.5);
	glNormal3f(-1.0f*dLength, 0.0f*0.25, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f*dLength, -0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f*dLength, 0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*dLength, 0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*dLength, -0.5f*0.25, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// bottom
	glColor3f(0.5f*dLength, 0.0f*0.25, 0.0f*0.5);
	glNormal3f(0.0f*dLength, -1.0f*0.25, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*dLength, -0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f*dLength, -0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*dLength, -0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f*dLength, -0.5f*0.25, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// back
	glColor3f(0.0f*dLength, 0.5f*0.25, 0.0f*0.5);
	glNormal3f(0.0f*dLength, 0.0f*0.25, -1.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*dLength, 0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*dLength, -0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*dLength, -0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*dLength, 0.5f*0.25, -0.5f*0.5);

	glEnd();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	
	//draw wheels with texture
	glPushMatrix();
	int i;
	int triangles = 20; // number of triangles

	float twoPi = 2.0f * 3.14159f;
	glTranslatef(-0.3, -0.5, 0.5);
	glRotatef(rAngle, 0, -1, 0);
	setTexture(&pix[0], 2067);
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(0, 0); // origin

	for (i = 0; i <= triangles; i++) {

		glVertex2f((0.3f * cos(i *  twoPi / triangles)),
			(0.3f * sin(i * twoPi / triangles)));
		glTexCoord2f((0.5f * cos(i *  twoPi / triangles)), (0.5f * cos(i *  twoPi / triangles)));
	}

	glEnd();
	glPopMatrix();
	
	glPushMatrix();
	
	glTranslatef(0.3, -0.5, 0.5);
	glRotatef(rAngle, 0, -1, 0);
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(0, 0); // origin

	for (i = 0; i <= triangles; i++) {

		glVertex2f((0.3f * cos(i *  twoPi / triangles)),
			(0.3f * sin(i * twoPi / triangles)));
		glTexCoord2f((0.5f * cos(i *  twoPi / triangles)), (0.5f * cos(i *  twoPi / triangles)));
	}

	glEnd();

	glPopMatrix();
	glPushMatrix();

	glTranslatef(0.3, -0.5, -0.5);
	glRotatef(rAngle, 0, -1, 0);
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(0, 0); // origin

	for (i = 0; i <= triangles; i++) {

		glVertex2f((0.3f * cos(i *  twoPi / triangles)),
			(0.3f * sin(i * twoPi / triangles)));
		glTexCoord2f((0.5f * cos(i *  twoPi / triangles)), (0.5f * cos(i *  twoPi / triangles)));
	}

	glEnd();

	glPopMatrix();
	glPushMatrix();

	glTranslatef(-0.3, -0.5, -0.5);
	glRotatef(rAngle, 0, -1, 0);
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(0, 0); // origin

	for (i = 0; i <= triangles; i++) {

		glVertex2f((0.3f * cos(i *  twoPi / triangles)),
			(0.3f * sin(i * twoPi / triangles)));
		glTexCoord2f((0.5f * cos(i *  twoPi / triangles)), (0.5f * cos(i *  twoPi / triangles)));
	}

	glEnd();

	glPopMatrix();
	glPopMatrix();

	




	glPushMatrix();
	
	glTranslatef(xMove2, yMove2+0.75, zMove2);
	if (health2 <= 100 && health2>90) {
		setTexture(&pix[3], 2005);
	}
	else if (health2<=90 && health2>50) {
		setTexture(&pix[5], 2008);
	}
	else if (health2 <= 50) {
		setTexture(&pix[6], 2010);
	}
	
	//draw base
	glPushMatrix();

	
	glBegin(GL_QUADS);
	glBegin(GL_QUADS);
	// top
	glColor3f(1.0f, 0.0f, 0.0f);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f, 0.5f, -0.5f);

	glEnd();

	glBegin(GL_QUADS);
	// front
	glColor3f(0.0f, 1.0f, 0.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f, -0.5f, 0.5f);

	glEnd();

	glBegin(GL_QUADS);
	// right
	glColor3f(0.0f, 0.0f, 1.0f);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f, 0.5f, -0.5f); 
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f, -0.5f, -0.5f);

	glEnd();

	glBegin(GL_QUADS);
	// left
	glColor3f(0.0f, 0.0f, 0.5f);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f, 0.5f, -0.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f, -0.5f, -0.5f);

	glEnd();

	glBegin(GL_QUADS);
	// bottom
	glColor3f(0.5f, 0.0f, 0.0f);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f, -0.5f, -0.5f);

	glEnd();

	glBegin(GL_QUADS);
	// back
	glColor3f(0.0f, 0.5f, 0.0f);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f, -0.5f, -0.5f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f, -0.5f, -0.5f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f, 0.5f, -0.5f);

	glEnd();
	glPopMatrix();

	//put all shoulder links in one big push pop
	glPushMatrix();

	glTranslatef(sMoveX2, sMoveY2, sMoveZ2);
	glRotatef(rAngle2, 0, -1, 0);
	glRotatef((GLfloat)shoulder2, 0.0, 0.0, 1.0);
	glTranslatef(sLength2 / 2, 0, 0);

	//draw shoulder
	glPushMatrix();

	glBegin(GL_QUADS);
	// top
	glColor3f(1.0f*sLength2, 0.0f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*sLength2, 1.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f*sLength2, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*sLength2, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f*sLength2, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*sLength2, 0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// front
	glColor3f(0.0f*sLength2, 1.0f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*sLength2, 0.0f*0.5, 1.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*sLength2, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*sLength2, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*sLength2, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*sLength2, -0.5f*0.5, 0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// right
	glColor3f(0.0f*sLength2, 0.0f*0.5, 1.0f*0.5);
	glNormal3f(1.0f*sLength2, 0.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*sLength2, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*sLength2, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f*sLength2, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f*sLength2, -0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// left
	glColor3f(0.0f*sLength2, 0.0f*0.5, 0.5f*0.5);
	glNormal3f(-1.0f*sLength2, 0.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f*sLength2, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f*sLength2, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*sLength2, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*sLength2, -0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// bottom
	glColor3f(0.5f*sLength2, 0.0f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*sLength2, -1.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*sLength2, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f*sLength2, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*sLength2, -0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f*sLength2, -0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// back
	glColor3f(0.0f*sLength2, 0.5f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*sLength2, 0.0f*0.5, -1.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*sLength2, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*sLength2, -0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*sLength2, -0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*sLength2, 0.5f*0.5, -0.5f*0.5);

	glEnd();
	glPopMatrix();

	//put elbow and shovel in one big push pop since they are linked 
	glPushMatrix();
	glTranslatef(eMoveX, eMoveY, eMoveZ);
	glRotatef((GLfloat)elbow2, 0.0, 0.0, 1.0);
	glTranslatef(eLength2 / 2, 0, 0);

	//draw elbow
	glPushMatrix();
	glBegin(GL_QUADS);
	// top
	glColor3f(1.0f*eLength2, 0.0f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*eLength2, 1.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f*eLength2, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*eLength2, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f*eLength2, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*eLength2, 0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// front
	glColor3f(0.0f*eLength2, 1.0f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*eLength2, 0.0f*0.5, 1.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*eLength2, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*eLength2, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*eLength2, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*eLength2, -0.5f*0.5, 0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// right
	glColor3f(0.0f*eLength2, 0.0f*0.5, 1.0f*0.5);
	glNormal3f(1.0f*eLength2, 0.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*eLength2, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*eLength2, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f*eLength2, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f*eLength2, -0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// left
	glColor3f(0.0f*eLength2, 0.0f*0.5, 0.5f*0.5);
	glNormal3f(-1.0f*eLength2, 0.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f*eLength2, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f*eLength2, 0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*eLength2, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*eLength2, -0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// bottom
	glColor3f(0.5f*eLength2, 0.0f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*eLength2, -1.0f*0.5, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*eLength2, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f*eLength2, -0.5f*0.5, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*eLength2, -0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f*eLength2, -0.5f*0.5, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// back
	glColor3f(0.0f*eLength2, 0.5f*0.5, 0.0f*0.5);
	glNormal3f(0.0f*eLength2, 0.0f*0.5, -1.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*eLength2, 0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*eLength2, -0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*eLength2, -0.5f*0.5, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*eLength2, 0.5f*0.5, -0.5f*0.5);

	glEnd();

	glPopMatrix();
	setTexture(&pix[4], 2005);
	//draw shovel
	glPushMatrix();
	glTranslatef(digMoveX2, digMoveY2, digMoveZ2);
	
	glTranslatef(dLength2 / 2, 0, 0);
	glBegin(GL_QUADS);
	// top
	glColor3f(1.0f*dLength2, 0.0f*0.25, 0.0f*0.5);
	glNormal3f(0.0f*dLength2, 1.0f*0.25, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f*dLength2, 0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*dLength2, 0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f*dLength2, 0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*dLength2, 0.5f*0.25, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// front
	glColor3f(0.0f*dLength2, 1.0f*0.25, 0.0f*0.5);
	glNormal3f(0.0f*dLength2, 0.0f*0.25, 1.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*dLength2, -0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*dLength2, 0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*dLength2, 0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*dLength2, -0.5f*0.25, 0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// right
	glColor3f(0.0f*dLength2, 0.0f*0.25, 1.0f*0.5);
	glNormal3f(1.0f*dLength2, 0.0f*0.25, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*dLength2, 0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*dLength2, 0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5f*dLength2, -0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f*dLength2, -0.5f*0.25, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// left
	glColor3f(0.0f*dLength2, 0.0f*0.25, 0.5f*0.5);
	glNormal3f(-1.0f*dLength2, 0.0f*0.25, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5f*dLength2, -0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f*dLength2, 0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*dLength2, 0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*dLength2, -0.5f*0.25, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// bottom
	glColor3f(0.5f*dLength2, 0.0f*0.25, 0.0f*0.5);
	glNormal3f(0.0f*dLength2, -1.0f*0.25, 0.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*dLength2, -0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5f*dLength2, -0.5f*0.25, 0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*dLength2, -0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5f*dLength2, -0.5f*0.25, -0.5f*0.5);

	glEnd();

	glBegin(GL_QUADS);
	// back
	glColor3f(0.0f*dLength2, 0.5f*0.25, 0.0f*0.5);
	glNormal3f(0.0f*dLength2, 0.0f*0.25, -1.0f*0.5);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.5f*dLength2, 0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.5f*dLength2, -0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.5f*dLength2, -0.5f*0.25, -0.5f*0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.5f*dLength2, 0.5f*0.25, -0.5f*0.5);

	glEnd();
	
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();

	//draw wheels with texture
	setTexture(&pix[0], 2067);
	glTranslatef(-0.3, -0.5, 0.5);
	glRotatef(rAngle2, 0, -1, 0);
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(0, 0); // origin

	for (i = 0; i <= triangles; i++) {
		
			glVertex2f((0.3f * cos(i *  twoPi / triangles)),
			(0.3f * sin(i * twoPi / triangles)));
		glTexCoord2f((0.5f * cos(i *  twoPi / triangles)), (0.5f * cos(i *  twoPi / triangles)));
	
	}

	glEnd();
	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.3, -0.5, 0.5);
	glRotatef(rAngle2, 0, -1, 0);
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(0, 0); // origin

	for (i = 0; i <= triangles; i++) {

		glVertex2f((0.3f * cos(i *  twoPi / triangles)),
			(0.3f * sin(i * twoPi / triangles)));
		glTexCoord2f((0.5f * cos(i *  twoPi / triangles)), (0.5f * cos(i *  twoPi / triangles)));
	}

	glEnd();

	glPopMatrix();
	glPushMatrix();

	glTranslatef(0.3, -0.5, -0.5);
	glRotatef(rAngle2, 0, -1, 0);
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(0, 0); // origin

	for (i = 0; i <= triangles; i++) {

		glVertex2f((0.3f * cos(i *  twoPi / triangles)),
			(0.3f * sin(i * twoPi / triangles)));
		glTexCoord2f((0.5f * cos(i *  twoPi / triangles)), (0.5f * cos(i *  twoPi / triangles)));
	}

	glEnd();

	glPopMatrix();
	glPushMatrix();

	glTranslatef(-0.3, -0.5, -0.5);
	glRotatef(rAngle2, 0, -1, 0);
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(0, 0); // origin

	for (i = 0; i <= triangles; i++) {

		glVertex2f((0.3f * cos(i *  twoPi / triangles)),
			(0.3f * sin(i * twoPi / triangles)));
		glTexCoord2f((0.5f * cos(i *  twoPi / triangles)), (0.5f * cos(i *  twoPi / triangles)));
	}

	glEnd();

	glPopMatrix();
	glPopMatrix();

	
	
	glutSwapBuffers();   // Double buffering, swap buffers

}


// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 6, (GLsizei)w, (GLsizei)h);


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
	if (health1 > 0 && health2>0) {
		if (key == 'f' || key == 'F')
		{
			if (speed > 50) speed -= 50;
			updateSpeed();
		}

		else if (key == 'b' || key == 'B')
		{
			if (speed < 250) speed += 50;
			updateSpeed();
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
	}

	//zoom key commands
	if (key == 'x' || key == 'X') {
		if(!fpvon)
		zoom += 5;
		reshape(vWidth, vHeight);
	}

	else if (key == 'z' || key == 'Z') {
		if (!fpvon)
		zoom -= 5;
		reshape(vWidth, vHeight);
	}
	else if (key == 'a' || key == 'A') {
		fpvon = true;
		
	}
	else if (key == 'v' || key == 'V') {
		fpvon = false;
	
	}


	glutPostRedisplay();   // Trigger a window redisplay
}

//This method calculates  the eye coordinates for gllookat
void movingMouse(int xMouse, int yMouse) {
	eX = -2 * dimension*sin(3.14 / 180 * xMouse) * cos(3.14 / 180 * yMouse);
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
		printf("- To Move the robot Left press 'L' or 'l'\n");
		printf("- To Move the robot Right press 'R' or 'r'\n");
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
		printf("- To enter FPV press 'a' or 'A'.\n");
		printf("- To exit FPV press 'v' or 'V'.\n");
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




