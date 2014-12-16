#include "main.h"

GLuint SkyboxTexture[6];
Camera objCamera;
float angle = 0.0;

unsigned char* loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}

	return rawData;
}

void loadTexture(const char* name, int id)
{
	//GLuint texture[1];     // storage for one texture
	int twidth, theight;   // texture width/height [pixels]
	unsigned char* tdata;  // texture pixel data

	// Load image file
	tdata = loadPPM(name, twidth, theight);
	if (tdata == NULL) return;

	// Create ID for texture
	glGenTextures(1, &SkyboxTexture[id]);

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, SkyboxTexture[id]);

	// Generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);

	// Make sure no bytes are padded:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Select GL_MODULATE to mix texture with polygon color for shading:
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Set bi-linear filtering for both minification and magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Draw_Skybox(float x, float y, float z, float width, float height, float length)
{
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
	// Center the Skybox around the given x,y,z position
	x = x - width / 2;
	y = y - height / 2;
	z = z - length / 2;


	// Draw Front side
	glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYFRONT]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y, z + length);
	glEnd();

	// Draw Back side
	glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYBACK]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y, z);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y + height, z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y, z);
	glEnd();

	// Draw Left side
	glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYLEFT]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y + height, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y, z);
	glEnd();

	// Draw Right side
	glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYRIGHT]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y, z);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y + height, z);
	glEnd();

	// Draw Up side
	glBindTexture(GL_TEXTURE_2D, SkyboxTexture[SKYUP]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y + height, z);
	glEnd();
}


void Draw_Bezier(float width, float height)
{
	glColor3f(0, 0, 1);

	Bezier my = Bezier(width, height, angle);
	for (float t1 = -0.5; t1 < 0.49; t1 += 0.01)
	{
		for (float t2 = -0.5; t2 < 0.49; t2 += 0.01)
		{
			my.tessellate(t1, t2, .1);
		}
	}
}

Matrix4 camView;
int c = 0;

void drawSphere(Vector4 p)
{
	if (c == 0)
	{
		Matrix4 translate;
		translate.makeTranslate(p.x, p.y, p.z);

		camView = objCamera.c;
		camView = camView*translate;
		camView.transpose();

		glLoadMatrixd(camView.getPointer());

		glPointSize(10.0);
		glBegin(GL_POINTS);
		glVertex3d(camView.m[0][3], camView.m[1][3], camView.m[2][3]);
		glEnd();
	}

	else
	{
		objCamera.e = Vector3(p.x, p.y, p.z);
		objCamera.getInverseCamera();
	}
}

Vector3 blc;
Vector3 brc;
Vector3 trc;
Vector3 tlc;

Bezier x1;
Bezier x2;
Bezier x3;
Bezier x4;

int cp = 0;

void setUpCurve(float d)
{
	blc = Vector3(-d, 0, -d);
	brc = Vector3(d, 0, -d);
	trc = Vector3(d, 0, d);
	tlc = Vector3(-d, 0, d);

	float dist = d / 3;

	if (cp == 1)
	{
		glColor3f(1, 0, 1);
		Matrix4 tc;
		Matrix4 t;

		t.makeTranslate(blc.x, blc.y, blc.z);
		tc = objCamera.c;
		tc = tc*t;
		tc.transpose();
		glLoadMatrixd(tc.getPointer());
		glutWireSphere(.5, 20, 20);

		t.makeTranslate(brc.x, brc.y, brc.z);
		tc = objCamera.c;
		tc = tc*t;
		tc.transpose();
		glLoadMatrixd(tc.getPointer());
		glutWireSphere(.5, 20, 20);

		t.makeTranslate(trc.x, trc.y, trc.z);
		tc = objCamera.c;
		tc = tc*t;
		tc.transpose();
		glLoadMatrixd(tc.getPointer());
		glutWireSphere(.5, 20, 20);

		t.makeTranslate(tlc.x, tlc.y, tlc.z);
		tc = objCamera.c;
		tc = tc*t;
		tc.transpose();
		glLoadMatrixd(tc.getPointer());
		glutWireSphere(.5, 20, 20);
	}

	glColor3f(0, 0, 1);
	x1 = Bezier(blc, Vector3(-dist, 0, -1.5*d), Vector3(dist, 0, -1.5*d), brc);
	glColor3f(1, 1, 0);
	x2 = Bezier(brc, Vector3(d*1.5, 0, -dist), Vector3(d*1.5, 0, dist), trc);
	glColor3f(1, 0, 0);
	x3 = Bezier(trc, Vector3(dist, 0, 1.5*d), Vector3(-dist, 0, 1.5*d), tlc);
	glColor3f(1, 1, 1);
	x4 = Bezier(tlc, Vector3(-1.5*d, 0, dist), Vector3(-1.5*d, 0, -dist), blc);
}

float i = 0;
float i2 = 0;
float i3 = 0;
float i4 = 0;

void draw()
{
	glColor3f(0, 1, 0);

	if (i < 1)
	{
		glColor3f(0, 1, 0);
		Vector4 test = x1.lip(i, .1);
		drawSphere(test);
		i += .005;
	}
	else if ( i >= 1 && i2 < 1)
	{
		Vector4 test = x2.lip(i2, .1);
		drawSphere(test);
		i2 += .005;
	}
	else if (i2 >= 1 && i3 < 1)
	{
		Vector4 test = x3.lip(i3, .1);
		drawSphere(test);
		i3 += .005;
	}
	else
	{
		Vector4 test = x4.lip(i4, .1);
		drawSphere(test);
		i4 += .005;
		if (i4 > .99)
		{
			i = 0;
			i2 = 0;
			i3 = 0;
			i4 = 0;
		}
	}
}

Matrix4 cs;

void drawDots(Vector4 p)
{
	Matrix4 translate;
	translate.makeTranslate(p.x, p.y, p.z);

	cs = objCamera.c;
	cs = cs*translate;
	cs.transpose();

	glLoadMatrixd(cs.getPointer());

	glPointSize(3.0);
	glBegin(GL_POINTS);
	glVertex3d(0, 0, 0);
	glEnd();
}

void drawPath()
{
	for (float g = 0; g < 1; g += .005)
	{
		glColor3f(0, 0, 1);
		Vector4 test = x1.lip(g, .1);
		drawDots(test);
	}
	for (float g = 0; g < 1; g += .005)
	{
		glColor3f(1, 1, 0);
		Vector4 test = x2.lip(g, .1);
		drawDots(test);
	}
	for (float g = 0; g < 1; g += .005)
	{
		glColor3f(1, 0, 0);
		Vector4 test = x3.lip(g, .1);
		drawDots(test);
	}
	for (float g = 0; g < 1; g += .005)
	{
		glColor3f(1, 1, 1);
		Vector4 test = x4.lip(g, .1);
		drawDots(test);
	}
}

int p = 0;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix

	glMatrixMode(GL_MODELVIEW);

	setUpCurve(4);

	Matrix4 cam = objCamera.c;
	cam.transpose();
	glLoadMatrixd(cam.getPointer());

	glColor3f(1, 1, 1);
	Draw_Skybox(0, 0, 0, 100, 100, 100);

	Matrix4 ct;
	ct.makeTranslate(-1, 0, 0);
	cam = objCamera.c;
	cam = ct * cam;
	cam.transpose();
	glLoadMatrixd(cam.getPointer());
	glColor3f(1, 0, 0);
	glutSolidCube(1);

	if (p == 1)
		drawPath();
	angle += .01;

	draw();

	glFlush();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	//objCamera.mWindowWidth = w;
	//objCamera.mWindowHeight = h;
}

void idle()
{
	display();
}

void initGL()
{
	glEnable(GL_TEXTURE_2D);   // enable texture mapping
	glShadeModel(GL_SMOOTH);   // enable smooth shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // black background
	glClearDepth(1.0f);        // depth buffer setup
	glEnable(GL_DEPTH_TEST);   // enables depth testing
	glDepthFunc(GL_LEQUAL);    // configure depth testing
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // really nice perspective calculations

	objCamera.setCamera(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));

	loadTexture("PalldioPalace_extern_front.ppm", SKYFRONT);
	loadTexture("PalldioPalace_extern_back.ppm", SKYBACK);
	loadTexture("PalldioPalace_extern_left.ppm", SKYLEFT);
	loadTexture("PalldioPalace_extern_right.ppm", SKYRIGHT);
	loadTexture("PalldioPalace_extern_top.ppm", SKYUP);
}

float theta = 0;

void keyboard(unsigned char key, int x, int y)
{
	Matrix4 rotate = objCamera.c;
	Matrix4 r;  
	Matrix4 testRotate;
	Vector3 test = objCamera.e;
	switch (key)
	{
	case 'b':
		objCamera.d = Vector3(objCamera.d.x, objCamera.d.y - 1, objCamera.d.z);
		objCamera.getInverseCamera();
		break;
	case 'B':
		objCamera.d = Vector3(objCamera.d.x, objCamera.d.y + 1, objCamera.d.z);
		objCamera.getInverseCamera();
		break;
	case 'y':
		objCamera.d = Vector3(objCamera.d.x-1, objCamera.d.y, objCamera.d.z-1);
		objCamera.getInverseCamera();
		break;
	case 'Y':
		objCamera.d = Vector3(objCamera.d.x + 1, objCamera.d.y, objCamera.d.z + 1);
		objCamera.getInverseCamera();
		break;
	case 'i':
		testRotate.makeRotateY(1.5);
		objCamera.c = testRotate*rotate;
		break;
	case 'I':
		testRotate.makeRotateY(-1.5);
		objCamera.c = testRotate*rotate;
		break;
	case 'u':
		testRotate.makeRotateX(-1.5);
		objCamera.c = testRotate*rotate;
		break;
	case 'U':
		testRotate.makeRotateX(1.5);
		objCamera.c = testRotate*rotate;
		break;
	case 'a':
		testRotate.makeTranslate(1, 0, 0);
		objCamera.c = testRotate*rotate;
		break;
	case 'd':
		testRotate.makeTranslate(-1, 0, 0);
		objCamera.c = testRotate*rotate;
		break;
	case 'w':
		testRotate.makeTranslate(0, -1, 0);
		objCamera.c = testRotate*rotate;
		break;
	case 's':
		testRotate.makeTranslate(0, 1, 0);
		objCamera.c = testRotate*rotate;
		break;
	case 'f':
		testRotate.makeTranslate(0, 0, 1);
		objCamera.c = testRotate*rotate;
		break;
	case 'F':
		testRotate.makeTranslate(0, 0, -1);
		objCamera.c = testRotate*rotate;
		break;
	case 'p':
		p = !p;
		break;
	case 'P':
		cp = !cp;
		break;
	case 'c':
		c = !c;
		break;
	case 'r':
		objCamera.setCamera(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	default:
		break;
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(512, 512);      // set initial window size
	glutCreateWindow("rollercoaster");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	initGL();

	glutMainLoop();
	return 0;
}