// standard
#include <assert.h>
#include <math.h>
#include <cmath>
#include <iostream>

// glut
#include <GL/glut.h>

// create instance
# define PI           3.14159265358979323846  /* pi */
using namespace std;
//================================
// global variables
//================================
// screen size
int g_screenWidth  = 0;
int g_screenHeight = 0;

// frame index
int g_frameIndex = 0;

// angle for rotation
int g_angle = 0;

// 4 control points for our cubic bezier curve
float Points[4][6] = {
	{ 4, 8, -8 , 10 ,5, 5 },
	{ -4, 2, 7 , 1, 1, 1 },
	{ 4, 2, -7 , 3 ,4, 20 },
	{ -4, -4, -1, 5, 5, 10 }
};

// Same orientation with euler
float QPoints[4][7] = {
	{ 4, 8, -8, 0.9941334603418283, 0.047210106163680564, -0.08509450499774092, 0.047210106163680564 },
	{ -4, 2, 7, 0.9998851089952057, 0.008802020473715273, -0.008649721429408268, 0.008802020473715273 },
	{ 4, 2, -7, 0.9837119328616093, 0.17438261050088974, -0.01970540030108529, 0.03890032775627564 },
	{ -4, -4, -1, 0.9941334603418283, 0.08888532658338241, -0.039613982669784595, 0.047210106163680564 }
};

// Individual test set
//float QPoints[4][7] = {
//	{ 4, 8, -8, 1, 2, 2, 2 },
//	{ -4, 2, 7, 1, 1, 1, 1 },
//	{ 4, 2, -7, 3, 2, 2, 2 },
//	{ -4, -4, -1, 1, 1, 1, 1 }
//};
// the level of detail of the curve
unsigned int LOD = 20;
// current time
float t = 0;
// Orientation representation
int g_R_type = 0;
// Spline representation
int g_S_type = 0;
float g_a;   // the input value for cardinal matrix
float b0, b1, b2, b3; // blending function values

//trans
float pos[7];
float Qpos[8];
//For matrix
float QTTRmatrix[16];
float ETTRmatrix[16];
//================================
// init
//================================
void init( void ) {
	// init something before main loop...
	printf("INSTRUCTION: 1. Euler Representation; 2. Quaternion Representation; \n Please enter number:  \n");
	cin >> g_R_type;
	if (g_R_type == 1){
		cout << "Euler system applied" << endl;
	}
	else{
		if (g_R_type == 2){
			cout << "Quaternion system applied" << endl;
		}
		else{
			cout << "Error: Invaild input value...." << endl;
			while (1);
		}
	}
	

	printf("INSTRUCTION: 1. Cardinal Spline; 2. Bspline; \n Please enter number:  \n");
	cin >> g_S_type;
	if (g_S_type == 1){
		cout << "Cardinal Curve applied \n Please enter the value of a: " << endl;
		cin >> g_a;
		printf("The value of a is %d  \n", g_a);
	}
	else{
		if (g_S_type == 2){
			cout << "Quaternion system applied" << endl;
		}
		else{
			cout << "Error: Invaild input value...." << endl;
			while (1);
		}
	}
	
}

//================================
// update
//================================
void update( void ) {
	// we will draw lots of little lines to make our curve
	// use the parametric time value 0 to 1
	if (t <= 1){
		t = t + 0.005;
	}
	else{
		t = 0;
	}
		
}

//================================
// render
//================================
void render( void ) {
	// clear buffer
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glClearDepth (1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	// render state
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	// enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// light source attributes
	GLfloat LightAmbient[]	= { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightDiffuse[]	= { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat LightSpecular[]	= { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightPosition[] = { 5.0f, 5.0f, 5.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT , LightAmbient );
	glLightfv(GL_LIGHT0, GL_DIFFUSE , LightDiffuse );
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	// surface material attributes
	GLfloat material_Ka[]	= { 0.11f, 0.06f, 0.11f, 1.0f };
	GLfloat material_Kd[]	= { 0.43f, 0.47f, 0.54f, 1.0f };
	GLfloat material_Ks[]	= { 0.33f, 0.33f, 0.52f, 1.0f };
	GLfloat material_Ke[]	= { 0.1f , 0.0f , 0.1f , 1.0f };
	GLfloat material_Se		= 10;

	glMaterialfv(GL_FRONT, GL_AMBIENT	, material_Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE	, material_Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR	, material_Ks);
	glMaterialfv(GL_FRONT, GL_EMISSION	, material_Ke);
	glMaterialf (GL_FRONT, GL_SHININESS	, material_Se);

	printf("current frame index is %d \n", g_frameIndex);
	float ttt = (float(g_frameIndex) / 60);
	printf("current time is %f \n", ttt);

	// modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -20.0);

	// draw the Control Vertices
	glColor3f(0, 1, 0);
	glPointSize(3);
	glBegin(GL_POINTS);
	for (int i = 0; i != 4; ++i) {
		glVertex3fv(Points[i]);
	}
	glEnd();

	// draw the hull of the curve
	glColor3f(1, 0.5, 0.25);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i != 4; ++i) {
		glVertex3fv(Points[i]);
	}
	glEnd();
		
	if (g_S_type == 1){
		// calculate blending functions
		// Cardinal Spline
		b0 = -g_a*t*t*t + 2.0 * g_a*t*t - g_a*t;
		b1 = (2.0 - g_a)*t*t*t + (g_a - 3.0)*t*t + 1;
		b2 = (g_a - 2.0)*t*t*t + (3.0 - 2.0 * g_a)*t*t + g_a*t;
		b3 = g_a*t*t*t - g_a*t*t;
	}else
	{
		// calculate blending functions
		// Bspline
		// nice to pre-calculate 1.0f-t because we will need it frequently
		float it = 1.0f - t;
		b0 = (it*t*t+ 3.0*t*t-3.0*t+1.0) / 6.0f;
		b1 = (3.0 * t*t*t - 6.0 * t*t + 4.0) / 6.0f;
		b2 = (-3.0 * t*t*t + 3.0 * t*t + 3.0 * t + 1.0) / 6.0f;
		b3 = t*t*t / 6.0f;
	}
		
		// calculate the x,y and z of the curve point by summing
		// the Control vertices weighted by their respective blending
		// functions
	if (g_R_type == 1){   /// Euler system applied
		// calculate interpolating values for Euler System
		float x = b0*Points[0][0] + b1*Points[1][0] + b2*Points[2][0] + b3*Points[3][0];

		float y = b0*Points[0][1] + b1*Points[1][1] + b2*Points[2][1] + b3*Points[3][1];

		float z = b0*Points[0][2] + b1*Points[1][2] + b2*Points[2][2] + b3*Points[3][2];

		float rx = b0*Points[0][3] + b1*Points[1][3] + b2*Points[2][3] + b3*Points[3][3];

		float ry = b0*Points[0][4] + b1*Points[1][4] + b2*Points[2][4] + b3*Points[3][4];

		float rz = b0*Points[0][5] + b1*Points[1][5] + b2*Points[2][5] + b3*Points[3][5];

		//Euler Rotation construction
		pos[0] = x; 		pos[1] = y; 		pos[2] = z;
		pos[3] = rx; 		pos[4] = ry; 		pos[5] = rz;

		float cox = cosf(rx); 		float six = sinf(rx);
		float coy = cosf(ry); 		float siy = sinf(ry);
		float coz = cosf(rz); 		float siz = sinf(rz);
		// Constructing Euler Roataion matrix
		float EulerRotation[9] = {
		coz*coy, (coz*siy*six) - (cox*siz), (siz*six) + (coz*cox*siy),
		coy*siz, (coz*cox) + (siz*siy*six), (cox*siz*siy) - (coz*six),
		-siy,		coy*six,		coy*cox
		};

		// Euler TRmatrix and its transpose
		float TRmatrix[16] = {
		EulerRotation[0], EulerRotation[1], EulerRotation[2], pos[0],
		EulerRotation[3], EulerRotation[4], EulerRotation[5], pos[1],
		EulerRotation[6], EulerRotation[7], EulerRotation[8], pos[2],
		0,					0,				0,					1
		};

		float ETTRmatrix[16] = {
		EulerRotation[0], EulerRotation[3], EulerRotation[6], 0,
		EulerRotation[1], EulerRotation[4], EulerRotation[7], 0,
		EulerRotation[2], EulerRotation[5], EulerRotation[8], 0,
		pos[0], pos[1], pos[2],1
		};
		glMultMatrixf(ETTRmatrix);
		// render objects
		glutSolidTeapot(1.0);
	}
	else{
		// Calculate interpolating values for Quaternion system
		float x = b0*Points[0][0] + b1*Points[1][0] + b2*Points[2][0] + b3*Points[3][0];

		float y = b0*Points[0][1] + b1*Points[1][1] + b2*Points[2][1] + b3*Points[3][1];

		float z = b0*Points[0][2] + b1*Points[1][2] + b2*Points[2][2] + b3*Points[3][2];

		float w = b0*Points[0][3] + b1*Points[1][3] + b2*Points[2][3] + b3*Points[3][3];

		float rx = b0*Points[0][4] + b1*Points[1][4] + b2*Points[2][4] + b3*Points[3][4];

		float ry = b0*Points[0][5] + b1*Points[1][5] + b2*Points[2][5] + b3*Points[3][5];

		float rz = b0*Points[0][6] + b1*Points[1][6] + b2*Points[2][6] + b3*Points[3][6];
		// GET interpolating values
		Qpos[0] = x; 		Qpos[1] = y; 		Qpos[2] = z;
		Qpos[3] = w; 		Qpos[4] = rx; 		Qpos[5] = ry; 		Qpos[6] = rz;
		// MAKE SURE w, x, y, z can match the w^w =x^2 = y^2 =z^2 = -1
		float c1 = cos(((rx / 180.0f)*PI) / 2.0f);
		float c2 = cos(((ry / 180.0f)*PI) / 2.0f);
		float c3 = cos(((rz / 180.0f)*PI) / 2.0f);
		float s1 = sin(((rx / 180.0f)*PI) / 2.0f);
		float s2 = sin(((ry / 180.0f)*PI) / 2.0f);
		float s3 = sin(((rz / 180.0f)*PI) / 2.0f);
		float nrx = (s1*c2*c3 - c1*s2*s3)*180.0 / PI;
		float nry = (c1*s2*c3 + s1*c2*s3)*180.0 / PI;
		float nrz = (c1*c2*s3 - s1*s2*c3)*180.0 / PI;
		float nw = acos(c1*c2*c3 + s1*s2*s3)*2.0*180.0 / PI;
		//printf( "%f ,  %f,  %f,  %f\n", w,x,y,z);
		//normalize vector
		float mag = sqrt(nrx*nrx + nry*nry + nrz*nrz + nw*nw);
		nrx /= mag;
		nry /= mag;
		nrz /= mag;
		nw /= mag;
		// Constructing the Quaternion rotation matrix
		float QuatRotation[9] = {
			1 - 2 * nry*nry - 2 * nrz*nrz, 2 * nrx*nry - 2 * nw*nrz, 2 * nrx*nrz + 2 * nw*nry,
			2 * nrx*nry + 2 * nw*nrz, 1 - 2 * nrx*nrx - 2 * nrz*nrz, 2 * nry*nrz - 2 * nw*nrx,
			2 * nrx*nrz - 2 * nw*nry, 2 * nry*nrz + 2 * nw*nrx, 1 - 2 * nrx*nrx - 2 * nry*nry
		};
		// Quat TRmatrix transposed
		float QTTRmatrix[16] = {
			QuatRotation[0], QuatRotation[3], QuatRotation[6], 0,
			QuatRotation[1], QuatRotation[4], QuatRotation[7], 0,
			QuatRotation[2], QuatRotation[5], QuatRotation[8], 0,
			Qpos[0], Qpos[1], Qpos[2], 1
		};
		glMultMatrixf(QTTRmatrix);
		// render objects
		glutSolidTeapot(1.0);
	}

	// disable lighting
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	// swap back and front buffers
	glutSwapBuffers();
}

//================================
// keyboard input
//================================
void keyboard( unsigned char key, int x, int y ) {
}

//================================
// reshape : update viewport and projection matrix when the window is resized
//================================
void reshape( int w, int h ) {
	// screen size
	g_screenWidth  = w;
	g_screenHeight = h;	
	
	// viewport
	glViewport( 0, 0, (GLsizei)w, (GLsizei)h );

	// projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 2000.0);
}


//================================
// timer : triggered every 16ms ( about 60 frames per second )
//================================
void timer( int value ) {	
	// increase frame index
	g_frameIndex++;
	

	update();
	
	// render
	glutPostRedisplay();

	// reset timer
	// 16 ms per frame ( about 60 frames per second )
	glutTimerFunc( 16, timer, 0 );
}

//================================
// main
//================================
int main( int argc, char** argv ) {
	// create opengL window
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH );
	glutInitWindowSize( 600, 600 ); 
	glutInitWindowPosition( 100, 100 );
	glutCreateWindow( argv[0] );

	// init
	init();
	
	// set callback functions
	glutDisplayFunc( render );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );
	glutTimerFunc( 16, timer, 0 );
	
	// main loop
	glutMainLoop();

	return 0;
}
