#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "sgi.h"

#define	WIRE 0		
#define	SHADE 1		
#define	TEX 2		

#define PI  3.14159265359
#define	fRadius 10			/* È¸Àü °¢µµ */

/*GLfloat mKa[4] = {0.11f, 0.06f, 0.11f, 1.0f}; 
GLfloat mKd[4] = {0.43f, 0.47f, 0.54f, 1.0f}; 
GLfloat mKs[4] = {0.33f, 0.33f, 0.52f, 1.0f}; 
GLfloat mKe[4] = {0.0f, 0.0f, 0.0f, 0.0f}; 
GLfloat shininess[1] = {50}; 
*/

GLfloat mKa[4] = { 0.2f, 0.2f, 0.2f, 0.0f }; /* Object : Ambient */
GLfloat mKd[4] = { 0.6f, 0.6f, 0.6f, 0.0f }; /* Object : Diffuse */
GLfloat mKs[4] = { 0.2f, 0.2f, 0.2f, 0.0f }; /* Object : Specular */
GLfloat mKe[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; /* Object : Emission */
GLfloat shininess[1] = { 50 }; /* Object : Shininess */

typedef struct {
	float x;
	float y;
	float z;
} Point;

typedef struct {
	unsigned long ip[3];
} Face;

int pnum;
int fnum;
Point *mpoint;
Face *mface;

int status=2;           // WIRE or SHADE  or TEXTURE
int cull=0;             // CULLING toggle 
char *fname="cube.dat";

Point Pos, Dir;
double rads=0.0;
int IsLoad=0;
int BirdEye=0;
int IsFull=0;

unsigned char * image = NULL;
int iwidth, iheight, idepth;

GLuint textureId[4];


void InitWalk(void)
{
	Pos.x=0;
	Pos.y=25;
	Pos.z=0;
}

void InitTexture(void)
{
	glGenTextures(1, &textureId[0]);
	glBindTexture(GL_TEXTURE_2D, textureId[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image =read_bmp("wallB.bmp", &iwidth, &iheight, &idepth);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &textureId[1]);
	glBindTexture(GL_TEXTURE_2D, textureId[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image =read_bmp("floor1.bmp", &iwidth, &iheight, &idepth);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);    
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &textureId[2]);
	glBindTexture(GL_TEXTURE_2D, textureId[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image =read_bmp("wallA.bmp", &iwidth, &iheight, &idepth);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void DrawWire(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glEnable (GL_DEPTH_TEST);
//	glEnable (GL_LIGHT0);
//	glEnable (GL_LIGHT1);
//	glEnable (GL_LIGHT2);
//	glEnable (GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCallList(1);
	glCallList(2);
	glutSwapBuffers();
}

void DrawShade(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);
//	glEnable (GL_DEPTH_TEST);
//	glEnable (GL_LIGHT0);
//	glEnable (GL_LIGHT1);
//	glEnable (GL_LIGHT2);
//	glEnable (GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(1);
	glCallList(2);
	glutSwapBuffers();
}

void DrawTexture(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_LIGHT0);
	glEnable (GL_LIGHT1);
	glEnable (GL_LIGHT2);
	glEnable (GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
//	glDisable(GL_COLOR_MATERIAL);

	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(1);
	glCallList(2);
	glutSwapBuffers();
}


Point cnormal(Point a, Point b, Point c)
{
    Point p, q, r;
    double val;
    p.x = a.x-b.x; p.y = a.y-b.y; p.z = a.z-b.z;
    q.x = c.x-b.x; q.y = c.y-b.y; q.z = c.z-b.z;

    r.x = p.y*q.z - p.z*q.y;
    r.y = p.z*q.x - p.x*q.z;
    r.z = p.x*q.y - p.y*q.x;

    val = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);
    r.x = r.x / val;
    r.y = r.y / val;
    r.z = r.z / val;
    return r;
}


void MakeGL_Model(void)
{
	int i;
	Point norm;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mKa);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mKd);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mKs);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mKe);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glShadeModel(GL_SMOOTH);

	if (glIsList(2)) glDeleteLists(2,2);			
	glNewList(2, GL_COMPILE);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, textureId[2]);

	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(80.0, 10.0, 0.0);
//	glTranslatef(0.0, 10.0, 0.0);
	glScalef(0.1,0.1,0.1);
	for (i = 0; i < fnum; i++) {
		norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]], 
			mpoint[mface[i].ip[0]]);
		glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 1.0f, 1.0f);

		glTexCoord2f(0.0, 0.0);
		glNormal3f(norm.x, norm.y, norm.z);
		glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y,
			mpoint[mface[i].ip[0]].z);
		if (i%2==0) glTexCoord2f(0.0, 1.0);
		else glTexCoord2f(1.0, 1.0);
		glNormal3f(norm.x, norm.y, norm.z);
		glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y,
			mpoint[mface[i].ip[1]].z);
		if (i % 2 == 0) glTexCoord2f(1.0, 1.0);
		else glTexCoord2f(1.0, 0.0);
		glNormal3f(norm.x, norm.y, norm.z);
		glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y,
			mpoint[mface[i].ip[2]].z);
		glEnd();
	}

	glPopMatrix();
	glEndList();
}

void MakeGL_GLUTModel(void)
{
	int i;
	Point norm;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mKa);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mKd);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mKs);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mKe);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glShadeModel(GL_SMOOTH);

	if (glIsList(2)) glDeleteLists(2, 2);
	glNewList(2, GL_COMPILE);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, textureId[2]);

	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(80.0, 10.0, 0.0);
	//	glutSolidTeapot(10.0);
	glutSolidSphere(10.0, 50, 50);

	/*	glScalef(0.1,0.1,0.1);
		for (i = 0; i < fnum; i++) {
			norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]],
				mpoint[mface[i].ip[0]]);
			glBegin(GL_TRIANGLES);
			glColor3f(1.0f, 1.0f, 1.0f);

			glNormal3f(norm.x, norm.y, norm.z);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y,
				mpoint[mface[i].ip[0]].z);
			if (i%2==0) glTexCoord2f(0.0, 1.0);
			else glTexCoord2f(1.0, 1.0);
			glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y,
				mpoint[mface[i].ip[1]].z);
			if (i % 2 == 0) glTexCoord2f(1.0, 1.0);
			else glTexCoord2f(1.0, 0.0);
			glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y,
				mpoint[mface[i].ip[2]].z);
			glEnd();
		}
	*/
	glPopMatrix();
	glEndList();
}


void MakeGL_SimpleModel(void)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mKa);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mKd);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mKs);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mKe);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glShadeModel(GL_SMOOTH);

	if (glIsList(1)) glDeleteLists(1,1);
	glNewList(1, GL_COMPILE);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, textureId[0]);

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
		glNormal3d(0,0,-1);
		glTexCoord2d(0,0);
   		glVertex3d(-100,0,100);  
		glTexCoord2d(0,1);
		glVertex3d(-100,50,100);
		glTexCoord2d(1,1);
		glVertex3d(100,50,100);
		glTexCoord2d(1,0);
		glVertex3d(100,0,100);
	glEnd();


	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
		glNormal3d(0,0,1);
		glTexCoord2d(0,0);
   		glVertex3d(100,0,-100);  
		glTexCoord2d(0,1);
		glVertex3d(100,50,-100);
		glTexCoord2d(1,1);
		glVertex3d(-100,50,-100);
		glTexCoord2d(1,0);
		glVertex3d(-100,0,-100);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);
		glNormal3d(1,0,0);
		glTexCoord2d(0,0);
   		glVertex3d(-100,0,-100);  
		glTexCoord2d(0,1);
		glVertex3d(-100,50,-100);
		glTexCoord2d(1,1);
		glVertex3d(-100,50,100);
		glTexCoord2d(1,0);
		glVertex3d(-100,0,100);
	glEnd();

	glColor3f(0.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
		glNormal3d(-1,0,0);
		glTexCoord2d(0,0);
		glVertex3d(100,0,100);
		glTexCoord2d(0,1);
		glVertex3d(100,50,100);
		glTexCoord2d(1,1);
		glVertex3d(100,50,-100);
		glTexCoord2d(1,0);
   		glVertex3d(100,0,-100);  
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textureId[1]);

	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
		glNormal3d(0,1,0);
		glTexCoord2d(0,0);
		glVertex3d(100,0,100);
		glTexCoord2d(0,2);
		glVertex3d(100,0,-100);
		glTexCoord2d(2,2);
		glVertex3d(-100,0,-100);
		glTexCoord2d(2,0);
   		glVertex3d(-100,0,100);  
	glEnd();

	glPopMatrix();
	glEndList();

}

void GLSetupLight(void)
{
	GLfloat lmodel_ambient[] = { 0.0, 0.0, 0.0, 1.0 };   // Ambient of the entire scene
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	/* Enable a Single Directional light. */
	GLfloat	light0_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };  // Ambient light 
	GLfloat light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };  // White diffuse light 
	GLfloat	light0_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // Specular light 
	GLfloat light0_position[] = { 1.0, 1.0, 1.0, 0.0 };  // Infinite light location 

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glEnable(GL_LIGHT0);

	GLfloat	light1_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };  // Ambient light 
	GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };  // White diffuse light 
	GLfloat	light1_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // Specular light 
	GLfloat light1_position[] = { 0.0, 1.0, 0.0, 1.0 };  // Point light location 

	// Enable a single OpenGL light. 
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0); // °¨¼èÇö»ó  (Constant: ca)
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);      // °¨¼èÇö»ó  (Linear: la)
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);   // °¨¼èÇö»ó  (Quadric: qa)
	glEnable(GL_LIGHT1);

	GLfloat light2_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat light2_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light2_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light2_position[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat spot_direction[] = { 0.0, 0.0, -1.0 };

	glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);     // ±¤¿ø ÁÖº¯±¤ (Intensity: I0)
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);       // ±¤¿ø ³­¹Ý»ç  (Intensity: I0)
	glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular); // ±¤¿ø Á¤¹Ý»ç  (Intensity: I0)
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);   // ±¤¿ø À§Ä¡      (Position: p)

	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0); // °¨¼èÇö»ó  (Constant: ca)
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.0);      // °¨¼èÇö»ó  (Linear: la)
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0);   // °¨¼èÇö»ó  (Quadric: qa)

	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction); // ÁýÁß±¤¿ø ¹æÇâ (Direction: d)
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 2.0);                     // ÁýÁß±¤¿ø (Falloff: sd)
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 45.0);                        // ÁýÁß±¤¿ø (Cutoff : sc)

	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHTING);
}

void GLSetupView(void) {
  /* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

  /* Setup the view */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
  
	gluPerspective(  40.0, // field of view in degree 
		1.0, // aspect ratio 
		1.0, // Z near 
		2000.0); // Z far 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Dir.x=Pos.x+(fRadius*cos(rads));
	Dir.y=Pos.y;
	Dir.z=Pos.z-(fRadius*sin(rads));

	if (BirdEye==0)  {
		gluLookAt(	Pos.x, Pos.y, Pos.z,
			Dir.x, Dir.y, Dir.z, 
			0.0f,		1.0f,		0.0f);
	}
	else {
		gluLookAt(	Pos.x, Pos.y+1000, Pos.z,
			Dir.x, Dir.y, Dir.z, 
			0.0f,		1.0f,		0.0f);
	}
}



void ReadModel()
{
	FILE *f1;
	char s[81];

	int i;

	if ((f1 = fopen(fname, "rt"))== NULL) {
		printf("No file\n");
		exit(0);
	}
	fscanf(f1,"%s",s);     printf( "%s", s );
	fscanf(f1,"%s",s);     printf( "%s", s );
	fscanf(f1,"%d",&pnum);     printf( "%d\n", pnum);

    mpoint = (Point*)malloc(sizeof(Point)*pnum);

	for (i=0; i<pnum; i++){
		fscanf(f1,"%f",&mpoint[i].x);
		fscanf(f1,"%f",&mpoint[i].y);
		fscanf(f1,"%f",&mpoint[i].z);
	    printf( "%f %f %f\n", mpoint[i].x, mpoint[i].y,mpoint[i].z);
	}

	fscanf(f1,"%s",s);     printf( "%s", s );
	fscanf(f1,"%s",s);     printf( "%s", s );
	fscanf(f1,"%d",&fnum);     printf( "%d\n", fnum);

	mface = (Face*)malloc(sizeof(Face)*fnum);
	for (i=0; i<fnum; i++){
		fscanf(f1,"%d",&mface[i].ip[0]);
		fscanf(f1,"%d",&mface[i].ip[1]);
		fscanf(f1,"%d",&mface[i].ip[2]);
	    printf("%d %d %d\n", mface[i].ip[0], mface[i].ip[1], mface[i].ip[2]);
	}
	fclose(f1);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLSetupView();
	if (IsLoad==0) {
		MakeGL_SimpleModel();
		ReadModel();
		MakeGL_Model();
		IsLoad=1;
	}

	if (status==WIRE) DrawWire();
	else if (status==SHADE) DrawShade();
	else DrawTexture();
}

void keyboard(unsigned char key, int x, int y)
{
	printf("key %d\n",key);
	switch (key)
	{
		case 'w':
			status=WIRE;
		    glutPostRedisplay();
			break;
		case 's':
			status=SHADE;
		    glutPostRedisplay();
			break;
		case 't':
			status=TEX;
		    glutPostRedisplay();
			break;
		case 'c':
			if (cull) cull=0;
			else cull=1;
		    glutPostRedisplay();
			break;
		case 'f':
			if (!IsFull) {
				glutFullScreen();
				printf("FullScreen\n");
				IsFull=1;
			}
			else {
				glutPositionWindow(100,100);
				glutReshapeWindow(400,400);
				printf("Window\n");
				IsFull=0;
			}
			break;

		case '0':
			Pos.x=Pos.z=0;
			Dir.x=Dir.z=0;
		    glutPostRedisplay();
			break;
		case '1':
			fname="cube.dat";
			ReadModel();
			IsLoad=0;
		    glutPostRedisplay();
			break;
		case '2':
			fname="sphere.dat";	
			ReadModel();
			IsLoad=0;
		    glutPostRedisplay();
			break;
		case '3':
			fname="teapot.dat";
			ReadModel();
			IsLoad=0;
		    glutPostRedisplay();
			break;
		case '4':
			MakeGL_GLUTModel();
			glutPostRedisplay();
			break;
		case 'v':
			if (!BirdEye) BirdEye=1;
			else BirdEye=0;
		    glutPostRedisplay();
			break;
		case 27:
			exit( EXIT_SUCCESS);
			break;

	}
}

void MoveViewer(GLdouble dStep)
{
	GLdouble xDelta,zDelta;
	xDelta = dStep*cos(rads);
	zDelta = -dStep*sin(rads);
	Pos.x += (float)xDelta;
	Pos.z += (float)zDelta;
}

void MoveStep(GLdouble dStep)
{
	GLdouble xDelta,zDelta;
	double frads;
	frads=PI*0.5-rads;
	xDelta = dStep*cos(frads);
	zDelta = dStep*sin(frads);
	Pos.x += (float)xDelta;
	Pos.z += (float)zDelta;
}

int Collision()
{
	printf("Pos %f %f %f\n",Pos.x, Pos.y, Pos.z);
	if (Pos.x> 80.0 || Pos.x <-80.0) return 1;
	if (Pos.z> 80.0 || Pos.z <-80.0) return 1;
	return 0;
}

void special(int key, int x, int y)
{

	switch (key)
	{
		case GLUT_KEY_LEFT :
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				printf("CTRL \n");
				MoveStep(-2.0);
				if (Collision()) MoveStep(2.5);
			}
			else 
			{
				rads += PI/60.0f; 
				if(rads > (2.0*PI))		
					rads = 0.0;
				printf("Left directional key. \n\n");
			}
			break;


		case GLUT_KEY_RIGHT :
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				printf("CTRL \n");
				MoveStep(2.0);
				if(Collision()) MoveStep(-2.5);
			}
			else 
			{
				printf("Right directional key. \n");
				rads -= PI/60.0; 
				if(rads < 0.0)
					rads = (2.0*PI);	
			}
			break;

		case GLUT_KEY_UP :
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				printf("CTRL \n");
				MoveViewer(4.0);					
				if(Collision()) MoveViewer(-6.0);					
				break;
			}
			else 
			{
				printf("Up directional key. \n");
				MoveViewer(2.0);					
				if(Collision()) MoveViewer(-2.5);					
				break;
			}
			
		case GLUT_KEY_DOWN :
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				printf("CTRL \n");
				MoveViewer(-4.0);					
				if(Collision()) MoveViewer(6.0);					
				break;
			}
			else 
			{
				printf("Down directional key. \n");
				MoveViewer(-2.0);					
				if(Collision()) MoveViewer(2.5);					
				break;
			}
		default :
			printf("Function key. \n");
			break;
	}


	display();

}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(400,400);
	glutInitWindowPosition(100,100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Simple Modeling");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	InitWalk();
	InitTexture();
	GLSetupLight();
	GLSetupView();
	glutMainLoop();
	return EXIT_SUCCESS;            /* ANSI C requires main to return int. */
}

