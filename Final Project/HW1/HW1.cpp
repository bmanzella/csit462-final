#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glui.h>
#include <string>
#include <math.h>
using namespace std;
#ifndef _WIN32
#include <unistd.h>
#else
void usleep(unsigned int nanosec)
{
	Sleep(nanosec / 1000);
}
#endif

int WINDOW_WIDTH = 400;
int WINDOW_HEIGHT = 400;
float rotateLeftArm = 0.0f;
float rotateRightArm = 0.0f;
float rotateHead = 0.0f;
const float PI = 3.14159;

float joint_rotLARM = 0.0f;
float joint_rotRARM = 0.0f;
float joint_rotLLEG = 0.0f;
float joint_rotRLEG = 0.0f;


// Window settings
int windowID;               // Glut window ID (for display)
GLUI *glui;                 // Glui window (for controls)
int Win[2];                 // window (x,y) size


// Animation settings
int animate_mode = 0;       // 0 = no anim, 1 = animate
int animation_frame = 0;      // Specify current frame of animation

// Joint parameters
const float JOINT_MIN = 0.0f;
const float JOINT_MAX = 360.0f;
float joint_rot = 0.0f;
float joint_x = 0;
float joint_y = 0;



enum {
	BRASS, RED_PLASTIC, EMERALD, SLATE
} MaterialType;
enum {
	TORUS_MATERIAL = 1, TEAPOT_MATERIAL = 2, ICO_MATERIAL = 3
} MaterialDisplayList;
enum {
	LIGHT_OFF, LIGHT_RED, LIGHT_WHITE, LIGHT_GREEN
} LightValues;

GLfloat red_light[] =
{ 1.0, 0.0, 0.0, 1.0 }, green_light[] =
{ 0.0, 1.0, 0.0, 1.0 }, white_light[] =
{ 1.0, 1.0, 1.0, 1.0 };
GLfloat left_light_position[] =
{ -1.0, 0.0, 1.0, 0.0 }, right_light_position[] =
{ 1.0, 0.0, 1.0, 0.0 };
GLfloat brass_ambient[] =
{ 0.33, 0.22, 0.03, 1.0 }, brass_diffuse[] =
{ 0.78, 0.57, 0.11, 1.0 }, brass_specular[] =
{ 0.99, 0.91, 0.81, 1.0 }, brass_shininess = 27.8;
GLfloat red_plastic_ambient[] =
{ 0.0, 0.0, 0.0 }, red_plastic_diffuse[] =
{ 0.5, 0.0, 0.0 }, red_plastic_specular[] =
{ 0.7, 0.6, 0.6 }, red_plastic_shininess = 32.0;
GLfloat emerald_ambient[] =
{ 0.0215, 0.1745, 0.0215 }, emerald_diffuse[] =
{ 0.07568, 0.61424, 0.07568 }, emerald_specular[] =
{ 0.633, 0.727811, 0.633 }, emerald_shininess = 76.8;
GLfloat slate_ambient[] =
{ 0.02, 0.02, 0.02 }, slate_diffuse[] =
{ 0.02, 0.01, 0.01 }, slate_specular[] =
{ 0.4, 0.4, 0.4 }, slate_shininess = .78125;

int shade_model = GL_SMOOTH;
char *left_light, *right_light;
char *ico_material, *teapot_material, *torus_material;

void animate()
{
	// Update geometry
	const double joint_rot_speed = 10;
	double joint_rot_t = double(int(animation_frame*joint_rot_speed) % int(JOINT_MAX)) / JOINT_MAX;
	joint_rot = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;

	glui->sync_live();


	//glutSetWindow(windowID);
	glutPostRedisplay();

	animation_frame++;
	usleep(50000);
}

void quitButton(int)
{
	exit(0);
}
void initGl(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
void animateButton(int)
{
	// synchronize variables that GLUT uses
	glui->sync_live();

	animation_frame = 0;
	if (animate_mode == 1) {
		// start animation
		GLUI_Master.set_glutIdleFunc(animate);
	}
	else {
		// stop animation
		GLUI_Master.set_glutIdleFunc(NULL);
	}
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

void output(GLfloat x, GLfloat y, char *format, ...)
{
	va_list args;
	char buffer[200], *p;

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	glPushMatrix();
	glTranslatef(x, y, 0);
	for (p = buffer; *p; p++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
	glPopMatrix();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(1.3, 1.3, 1.3);
	glRotatef(20.0, 1.0, 0.0, 0.0);
	glPushMatrix();
	glTranslatef(-0.65, 0.7, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.75, -0.8, 0.0);
	glCallList(TEAPOT_MATERIAL);
	head();
	body();
	leftArm();
	rightArm();
	leftLeg();
	rightLeg();
	glPopMatrix();
	glPushMatrix();

	glPopMatrix();
	glPopMatrix();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 3000, 0, 3000);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	glutSwapBuffers();
}

void light_select(GLenum which, int value, char **label)
{
	glEnable(which);
	switch (value) {
	case LIGHT_OFF:
		*label = "off";
		glDisable(which);
		break;
	case LIGHT_RED:
		*label = "red";
		glLightfv(which, GL_DIFFUSE, red_light);
		break;
	case LIGHT_WHITE:
		*label = "white";
		glLightfv(which, GL_DIFFUSE, white_light);
		break;
	case LIGHT_GREEN:
		*label = "green";
		glLightfv(which, GL_DIFFUSE, green_light);
		break;
	}
	glutPostRedisplay();
}

void left_light_select(int value)
{
	light_select(GL_LIGHT0, value, &left_light);
}

void right_light_select(int value)
{
	light_select(GL_LIGHT1, value, &right_light);
}

void material(int dlist, GLfloat * ambient, GLfloat * diffuse,
	GLfloat * specular, GLfloat shininess)
{
	glNewList(dlist, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glEndList();
}

char * material_select(int object, int value)
{
	glutPostRedisplay();
	switch (value) {
	case BRASS:
		material(object, brass_ambient,
			brass_diffuse, brass_specular, brass_shininess);
		return "brass";
	case RED_PLASTIC:
		material(object, red_plastic_ambient, red_plastic_diffuse,
			red_plastic_specular, red_plastic_shininess);
		return "red plastic";
	case EMERALD:
		material(object, emerald_ambient, emerald_diffuse,
			emerald_specular, emerald_shininess);
		return "emerald";
	case SLATE:
		material(object, slate_ambient, slate_diffuse,
			slate_specular, slate_shininess);
		return "slate";
	}
	return NULL; /* avoid bogus warning! */
}

void main_menu_select(int value)
{
	if (value == 666)
		exit(0);
	glShadeModel(shade_model = value);
	glutPostRedisplay();
}
void resize(int x, int y)
{
	double aspect = WINDOW_WIDTH / WINDOW_HEIGHT;

	if (aspect > (double)x / y)
		glViewport(0, 0, x, x / aspect);
	else
		glViewport(0, 0, y*aspect, y);
}
void initGlut(char* winName)
{
	// Set video mode: double-buffered, color, depth-buffered

	glLightfv(GL_LIGHT0, GL_POSITION, left_light_position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT1, GL_POSITION, right_light_position);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white_light);
	left_light_select(LIGHT_RED);
	right_light_select(LIGHT_GREEN);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glMatrixMode(GL_PROJECTION);
	gluPerspective( /* degrees field of view */ 50.0,
		/* aspect ratio */ 1.0, /* Z near */ 1.0, /* Z far */ 10.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
		0.0, 0.0, 0.0,      /* center is at (0,0,0) */
		0.0, 1.0, 0.);      /* up is in positive Y direction */
	glTranslatef(0.0, 0.0, -1.0);
}
void initGlui()
{
	GLUI_Master.set_glutIdleFunc(NULL);

	glutSetWindow(windowID);
	// Create GLUI window
	glui = GLUI_Master.create_glui("Glui Window", 0, Win[0] + 10, 0);

	// Create a control to specify the rotation of the joint
	GLUI_Spinner *joint_spinner
		= glui->add_spinner("Left Arm", GLUI_SPINNER_FLOAT, &joint_rotLARM);
	joint_spinner->set_speed(0.1);
	joint_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

	GLUI_Spinner *joint_spinnerR
		= glui->add_spinner("Right Arm", GLUI_SPINNER_FLOAT, &joint_rotRARM);
	joint_spinnerR->set_speed(0.1);
	joint_spinnerR->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

	GLUI_Spinner *joint_spinnerE
		= glui->add_spinner("Left Leg", GLUI_SPINNER_FLOAT, &joint_rotLLEG);
	joint_spinnerE->set_speed(0.1);
	joint_spinnerE->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

	GLUI_Spinner *joint_spinnerD
		= glui->add_spinner("Right Leg", GLUI_SPINNER_FLOAT, &joint_rotRLEG);
	joint_spinnerD->set_speed(0.1);
	joint_spinnerD->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

	// Add button to specify animation mode
	glui->add_separator();
	glui->add_checkbox("Animate", &animate_mode, 0, animateButton);

	// Add "Quit" button
	glui->add_separator();
	glui->add_button("Quit", 0, quitButton);

	// Set the main window to be the "active" window
	glui->set_main_gfx_window(windowID);
}
int main(int argc, char **argv)
{
	// Process program arguments
	if (argc != 3) {
		Win[0] = 400;
		Win[1] = 400;
	}
	else {
		Win[0] = atoi(argv[1]);
		Win[1] = atoi(argv[2]);
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	windowID = glutCreateWindow("CSIT462 Computer Graphics Final");
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(Win[0], Win[1]);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	initGlut(argv[0]);
	initGlui();
	initGl();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutMainLoop();
	return 0;
}