#include "framebuff.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define round(fp) (int)((fp) >= 0 ? (fp) + 0.5 : (fp) - 0.5)

#define FPS 30
#define T 2000000/FPS

//  Initialization
void init ();

//  Callback functions
void display (void);
void mouse (int button, int state, int x, int y);
void keyboard (unsigned char key, int x, int y);

//  Support Functions
void centerOnScreen ();

//  define the window position on screen
int window_x;
int window_y;

//  variable representing the window title
char *window_title = "UTH YUV Player by Petros Kalos";

//  Tells whether to display the window full screen or not
//  Press Alt + Esc to exit a full screen.
int full_screen = 0;
int frameNUM;
int FPSCnt=0;
char FPSStr[100];
int play = 0;
int delay = T;

LARGE_INTEGER frequency,t1,t2;
__int64 fps1,fps2;

void init ()
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

__inline void uSleep(int waitTime) {
    __int64 time1 = 0, time2 = 0;
    QueryPerformanceCounter((LARGE_INTEGER *) &time1);
    do{
        QueryPerformanceCounter((LARGE_INTEGER *) &time2);
    }while((time2-time1) < waitTime);
}

void display (void)
{
	double time;
	
	uSleep(T-delay);

	QueryPerformanceCounter((LARGE_INTEGER *) &fps1);
	getNextFrame();
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB,GL_UNSIGNED_BYTE, buffer);
	glutSwapBuffers ();

	if(play){
		glutPostRedisplay ();
		if(frameNUM==0) QueryPerformanceCounter(&t1);
		QueryPerformanceCounter(&t2);
		time = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
		FPSCnt++;
		if(time>=1000){
			sprintf(FPSStr, "%d/%.0lf",FPSCnt,time);
			glutSetWindowTitle(FPSStr);
			FPSCnt=0;
			QueryPerformanceCounter(&t1);
		}
		frameNUM++;
		QueryPerformanceCounter((LARGE_INTEGER *) &fps2);
		delay = fps2 - fps1;
	}
	
}

void mouse (int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		frameNUM++;;
		glutPostRedisplay ();
	}
}

void keyboard (unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'p':
		play = 1;
		glutPostRedisplay ();
		break;
	case 's':
		play = 0;
		break;
	case 'a':
		play = 0;
		//generateImage(frameNUM--);
		glutPostRedisplay ();
		break;
	case 'd':
		play = 0;
		//generateImage(frameNUM++);
		glutPostRedisplay ();
		break;
	case 27:
		exit (0);
	}
}

void centerOnScreen ()
{
	window_x = (glutGet (GLUT_SCREEN_WIDTH) - WINDOW_WIDTH)/2;
	window_y = (glutGet (GLUT_SCREEN_HEIGHT) - WINDOW_HEIGHT)/2;
}

void initMemory(int argc, char **argv){
	int i,j;
	QueryPerformanceFrequency(&frequency);
	WINDOW_WIDTH = (unsigned int)atoi(argv[2]);
	WINDOW_HEIGHT = (unsigned int)atoi(argv[3]);

	startFrameReader(0);

	buffer = (pixel *)malloc(sizeof(pixel)*WINDOW_HEIGHT*WINDOW_WIDTH);
}

int main (int argc, char **argv)
{
	initFrame(argc,argv);
	initMemory(argc, argv);

	//  Connect to the windowing system
	glutInit(&argc, argv);

	//  create a window with the specified dimensions
	glutInitWindowSize (WINDOW_WIDTH, WINDOW_HEIGHT);

	//  Set the window x and y coordinates such that the
	//  window becomes centered
	centerOnScreen ();

	//  Position Window
	glutInitWindowPosition (window_x, window_y);

	//  Set Display mode
	glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE);

	//  Create window with the specified title
	glutCreateWindow (window_title);

	//  View in full screen if the full_screen flag is on
	if (full_screen)
		glutFullScreen ();

	//  Set OpenGL program initial state.
	init();

	// Set the callback functions
	glutDisplayFunc (display);
	glutKeyboardFunc (keyboard);
	glutMouseFunc (mouse);

	//  Start GLUT event processing loop
	glutMainLoop();
}