#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include "header.h"

#define MAX 65025

unsigned int SIZE;
unsigned int FRAMES;

yuv_buffer *yuv;
rgb *rgb_out8;

FILE *fp;

int parseFile(int frame){

	if(yuv->y==NULL || yuv->u==NULL || yuv->v==NULL) return 0;	

	fseek(fp,(yuv->y_height*yuv->y_width+2*yuv->uv_height*yuv->uv_width)*frame,SEEK_SET);

	fread(yuv->y,sizeof(unsigned char),yuv->y_height*yuv->y_width,fp);
	fread(yuv->u,sizeof(unsigned char),yuv->uv_height*yuv->uv_width,fp);
	fread(yuv->v,sizeof(unsigned char),yuv->uv_height*yuv->uv_width,fp);

	return 1;
}

unsigned int getFileSize(const char *filename){
	struct stat st;
	stat(filename, &st);
	return(st.st_size);
}

void writeYUV(yuv_buffer *yuv,const char *name){
	FILE *fp = fopen(name,"wb");

	fwrite(yuv->y,sizeof(unsigned char),yuv->y_height*yuv->y_width,fp);
	fwrite(yuv->u,sizeof(unsigned char),yuv->uv_height*yuv->uv_width,fp);
	fwrite(yuv->v,sizeof(unsigned char),yuv->uv_height*yuv->uv_width,fp);

	fclose(fp);
}

void initFrame(int argc,char *argv[]){

	fp = fopen(argv[1],"rb");

	yuv = (yuv_buffer *) malloc(sizeof(yuv_buffer));

	yuv->y_width = (unsigned int)atoi(argv[2]);
	yuv->y_height = (unsigned int)atoi(argv[3]);

	initRGB(yuv->y_width,yuv->y_height);

	yuv->y_stride = yuv->y_width;

	yuv->uv_width = yuv->y_width/2;
	yuv->uv_height = yuv->y_height/2;
	yuv->uv_stride = yuv->y_width/2;

	yuv->y = (unsigned char *)malloc(sizeof(unsigned char)*yuv->y_height*yuv->y_width);
	yuv->u = (unsigned char *)malloc(sizeof(unsigned char)*yuv->uv_height*yuv->uv_width);
	yuv->v = (unsigned char *)malloc(sizeof(unsigned char)*yuv->uv_height*yuv->uv_width);

	FRAMES = (int)(getFileSize(argv[1])/(yuv->y_width*yuv->y_height*1.5));

	rgb_out8 = (rgb *) malloc(sizeof(rgb));

	rgb_out8->r = (unsigned char *)malloc(sizeof(unsigned char)*yuv->y_height*yuv->y_width);
	rgb_out8->g = (unsigned char *)malloc(sizeof(unsigned char)*yuv->y_height*yuv->y_width);
	rgb_out8->b = (unsigned char *)malloc(sizeof(unsigned char)*yuv->y_height*yuv->y_width);

	rgb_out8->h = yuv->y_height;
	rgb_out8->w = yuv->y_width;
}

int getFrame(rgb *rgb_out,unsigned int frame){

	parseFile(frame);
	yuv420torgb_8(rgb_out,yuv);

	if(frame>=FRAMES){ return 1;}

	return 0;
}