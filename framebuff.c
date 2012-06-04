#include "framebuff.h"

#include <strsafe.h>
#include <stdio.h>

#define BUFFSIZE 10
CRITICAL_SECTION cs;

HANDLE frameReaderThread = 0;
HANDLE Mutex;                 //Semaphore Mutex,to ensure mutual excute   
HANDLE Full, Empty;           //Semaphore Full,to indicate the buffer HAS product or not

int frameNum=0;
int write_c=0;                 //to remeber total number of products in buffer
int read_c=0;


rgb buff[BUFFSIZE];


void getNextFrame(){
	int i,j;
	WaitForSingleObject(Full,INFINITE);
	EnterCriticalSection(&cs);
		for (i = 0; i < WINDOW_HEIGHT; i++) {
			for (j = 0; j < WINDOW_WIDTH; j++) {
				buffer[(WINDOW_HEIGHT-i-1)*WINDOW_WIDTH+j].r = (GLubyte) buff[read_c].r[i*WINDOW_WIDTH+j];
				buffer[(WINDOW_HEIGHT-i-1)*WINDOW_WIDTH+j].g = (GLubyte) buff[read_c].g[i*WINDOW_WIDTH+j];
				buffer[(WINDOW_HEIGHT-i-1)*WINDOW_WIDTH+j].b = (GLubyte) buff[read_c].b[i*WINDOW_WIDTH+j];
			}
		}
		read_c = (read_c+1) % BUFFSIZE;
	LeaveCriticalSection(&cs);
	ReleaseSemaphore(Empty,1,NULL); 
}

DWORD WINAPI readerThread( LPVOID lpParam ) 
{
	int i,j;
	
	while(1){		
		WaitForSingleObject(Empty,INFINITE);
		EnterCriticalSection(&cs);
			i = getFrame(&buff[write_c],frameNum++);
			if(i) exit(0);
			write_c = (write_c + 1) % BUFFSIZE;
		LeaveCriticalSection(&cs);
		ReleaseSemaphore(Full,1,NULL); 
	};  
	return 0; 
}

void startFrameReader(int frame){
	int i;
	InitializeCriticalSectionAndSpinCount(&cs,1000);
	for(i=0;i<BUFFSIZE;i++){
		buff[i].r = (unsigned char *)malloc(sizeof(unsigned char)*WINDOW_HEIGHT*WINDOW_WIDTH);
		buff[i].g = (unsigned char *)malloc(sizeof(unsigned char)*WINDOW_HEIGHT*WINDOW_WIDTH);
		buff[i].b = (unsigned char *)malloc(sizeof(unsigned char)*WINDOW_HEIGHT*WINDOW_WIDTH);

	}

	Mutex=CreateMutex(NULL,FALSE,NULL);               //Create Semaphores

	Full=CreateSemaphore(NULL,0,BUFFSIZE,NULL);  
	Empty=CreateSemaphore(NULL,BUFFSIZE,BUFFSIZE,NULL);  
	
	frameNum = frame;

	// Create Frame Reader Thread.
	frameReaderThread = CreateThread( NULL, 0, readerThread, NULL, 0, NULL);  
	if (frameReaderThread == NULL)	ExitProcess(0);

}