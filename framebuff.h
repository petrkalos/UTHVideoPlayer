#include "header.h"

#include <Windows.h>
#include <gl/glut.h>
#include <gl/GLU.h>
#include <gl/GL.h>

typedef struct {
	GLubyte r, g, b;
} pixel;

int WINDOW_HEIGHT;
int WINDOW_WIDTH;

void startFrameReader(int frame);
void getNextFrame();

//  Represents the pixel buffer in memory
pixel *buffer;
