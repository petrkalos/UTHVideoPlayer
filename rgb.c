#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "header.h"

#define MSB4 0xF0
#define N 4
const unsigned char d4[N][N]= {{15,7,13,5},{3,11,1,9},{12,4,14,6},{0,8,2,10}};
yuv_buffer *yuv444;

__inline char clip(int x){
	if(x>255) x = 255;
	if(x<0) x = 0;
	return x;
}

__inline char clip4(int x){
	if(x>255) x = 255;
	if(x<0) x = 0;

	x=x&MSB4;

	return x;
}

void dithering(rgb *rgb_in){
	unsigned int i,j;
	int t;
	for(i=0;i<rgb_in->h;i++){
		for(j=0;j<rgb_in->w;j++){
			
			t = rgb_in->r[i*rgb_in->w+j] + d4[i%N][j%N];	
			rgb_in->r[i*rgb_in->w+j] = clip(t);
			
			t = rgb_in->g[i*rgb_in->w+j] + d4[i%N][j%N];	
			rgb_in->g[i*rgb_in->w+j] = clip(t);

			t = rgb_in->b[i*rgb_in->w+j] + d4[i%N][j%N];	
			rgb_in->b[i*rgb_in->w+j] = clip(t);
			
		}
	}
}

int etheora_420to444( yuv_buffer *yuv420,  yuv_buffer *yuv444){
	int i, j,index420,index444;
	
	/*copying Y
	for (i = 0; i  < yuv444->y_width; i++){
		for (j = 0; j < yuv444->y_height; j++){
			index420 = i + (yuv420->y_stride)*(j);
			index444 = i + (yuv444->y_stride)*(j);			
			yuv444->y[index444] = yuv420->y[index420];
		}
	}*/

	yuv444->y = yuv420->y;

	/*upsampling CbCr*/
	for (i = 0; i  < yuv444->uv_width; i++){
		for (j = 0; j < yuv444->uv_height-1; j++){
			index420 = (i/2) + (yuv420->uv_stride)*(j/2);
			index444 =   i   + (yuv444->uv_stride) * j; 
			/*even rows in CbCr plane:*/
			yuv444->u[index444]     = yuv420->u[index420]; 
			yuv444->u[index444 +1 ] = yuv420->u[index420];
			yuv444->v[index444]     = yuv420->v[index420]; 
			yuv444->v[index444 +1 ] = yuv420->v[index420]; 
			/*odd rows in CbCr plane are equal to the even
			ones:*/
			index444 = i-1 + (yuv444->uv_stride) * (j+1); 			

			yuv444->u[index444]     = yuv420->u[index420]; 
			yuv444->u[index444 +1 ] = yuv420->u[index420]; 
			yuv444->v[index444]     = yuv420->v[index420]; 
			yuv444->v[index444 +1 ] = yuv420->v[index420];
			/* (Note +1 makes odd columns in CbCr plane
			equal to the even ones.)*/
		}
	}
	return 0; 
}

int etheora_444to420(yuv_buffer *yuv444, yuv_buffer *yuv420){
	int i, j; 
	int index420,index444;
	
	//copying Y
	for (i = 0; i  < yuv444->y_width; i++){
		for (j = 0; j < yuv444->y_height ; j++){

			index420 = i + (yuv420->y_stride)*(j); 
			index444 = i + (yuv444->y_stride)*(j); 

			yuv420->y[index420] = yuv444->y[index444]; 
		}
	}

	

	/*downsampling CbCr*/
	for (i = 0; i  < yuv444->uv_width; i=i+1){
		for (j = 0; j < yuv444->uv_height; j=j+1){
			index420 = (i/2) + (yuv420->uv_stride)*(j/2);
			index444 =   i   +  yuv444->uv_stride *  j; 

			yuv420->u[index420] = yuv444->u[index444]; 
			yuv420->v[index420] = yuv444->v[index444]; 
		}
	/*TODO: this loop is accessing the same yuv420->x[index] 
	  more than once. should be optmized.*/
	}
	return 0; 
}

void yuv420torgb_8(rgb *rgb_out,yuv_buffer *yuv420){
	unsigned int w = yuv420->y_width;
	unsigned int h = yuv420->y_height;
	int c,d,e;
	int i;

	etheora_420to444(yuv420,yuv444);

	for(i=0;i<h*w;i++){
		c = yuv444->y[i] - 16;
		d = yuv444->u[i] - 128;
		e = yuv444->v[i] - 128;

		rgb_out->r[i] = clip(( 298 * c           + 409 * e + 128) >> 8);
		rgb_out->g[i] = clip(( 298 * c - 100 * d - 208 * e + 128) >> 8);
		rgb_out->b[i] = clip(( 298 * c + 516 * d           + 128) >> 8);
	}

}

yuv_buffer *rgbtoyuv420_8(rgb *rgb_in){
	int i,h,w;
	yuv_buffer *yuv444;
	yuv_buffer *yuv420;

	h = rgb_in->h;
	w = rgb_in->w;
	
	yuv420 = (yuv_buffer *) malloc(sizeof(yuv_buffer));
	yuv444 = (yuv_buffer *) malloc(sizeof(yuv_buffer));
	
	yuv420->y_height= h;	yuv420->uv_height	= h/2;
	yuv420->y_width	= w;	yuv420->uv_width	= w/2;
	yuv420->y_stride= w;	yuv420->uv_stride	= w/2;	
	
	yuv444->y_height = h;	yuv444->uv_height = h;
	yuv444->y_width = w;	yuv444->uv_width = w;
	yuv444->y_stride = w;	yuv444->uv_stride=w;

	yuv420->y = (unsigned char *)malloc(sizeof(unsigned char)*yuv420->y_height*yuv420->y_width);
	yuv420->u = (unsigned char *)malloc(sizeof(unsigned char)*yuv420->uv_height*yuv420->uv_width);
	yuv420->v = (unsigned char *)malloc(sizeof(unsigned char)*yuv420->uv_height*yuv420->uv_width);
	
	yuv444->y = (unsigned char *)malloc(sizeof(unsigned char)*yuv444->y_height*yuv444->y_width);
	yuv444->u = (unsigned char *)malloc(sizeof(unsigned char)*yuv444->uv_height*yuv444->uv_width);
	yuv444->v = (unsigned char *)malloc(sizeof(unsigned char)*yuv444->uv_height*yuv444->uv_width);
	
	for(i=0;i<h*w;i++){
		yuv444->y[i] = ( (  66 * rgb_in->r[i] + 129 * rgb_in->g[i] +  25 * rgb_in->b[i] + 128) >> 8) +  16;
		yuv444->u[i] = ( ( -38 * rgb_in->r[i] -  74 * rgb_in->g[i] + 112 * rgb_in->b[i] + 128) >> 8) + 128;
		yuv444->v[i] = ( ( 112 * rgb_in->r[i] -  94 * rgb_in->g[i] -  18 * rgb_in->b[i] + 128) >> 8) + 128;
	}

	etheora_444to420(yuv444,yuv420);

	return yuv420;
}

rgb *yuv420torgb_4(yuv_buffer *yuv420,int dither){
	unsigned int w = yuv420->y_width;
	unsigned int h = yuv420->y_height;
	int c,d,e;
	int i;

	rgb *rgb_out;
	yuv_buffer *yuv444;

	rgb_out = (rgb *)malloc(sizeof(rgb));

	rgb_out->r = (unsigned char *)malloc(sizeof(unsigned char)*w*h);
	rgb_out->g = (unsigned char *)malloc(sizeof(unsigned char)*w*h);
	rgb_out->b = (unsigned char *)malloc(sizeof(unsigned char)*w*h);


	yuv444 = (yuv_buffer *)malloc(sizeof(yuv_buffer));

	yuv444->y_height = h;		yuv444->uv_height = h;
	yuv444->y_width = w;		yuv444->uv_width = w;
	yuv444->y_stride = w;		yuv444->uv_stride = w; 

	yuv444->y = (unsigned char *)malloc(sizeof(unsigned char)*yuv444->y_height*yuv444->y_width);
	yuv444->u = (unsigned char *)malloc(sizeof(unsigned char)*yuv444->uv_height*yuv444->uv_width);
	yuv444->v = (unsigned char *)malloc(sizeof(unsigned char)*yuv444->uv_height*yuv444->uv_width);

	etheora_420to444(yuv420,yuv444);

	for(i=0;i<h*w;i++){
		c = yuv444->y[i] - 16;
		d = yuv444->u[i] - 128;
		e = yuv444->v[i] - 128;

		rgb_out->r[i] = clip4(( 298 * c           + 409 * e + 128) >> 8);
		rgb_out->g[i] = clip4(( 298 * c - 100 * d - 208 * e + 128) >> 8);
		rgb_out->b[i] = clip4(( 298 * c + 516 * d           + 128) >> 8);
	}

	rgb_out->w = w;
	rgb_out->h = h;
	
	if(dither==1) dithering(rgb_out);

	free(yuv444->y);
	free(yuv444->u);
	free(yuv444->v);
	free(yuv444);

	return rgb_out;
}

yuv_buffer *rgbtoyuv420_4(rgb *rgb_in){
	int i,h,w;
	yuv_buffer *yuv444;
	yuv_buffer *yuv420;

	h = rgb_in->h;
	w = rgb_in->w;
	
	yuv420 = (yuv_buffer *) malloc(sizeof(yuv_buffer));
	yuv444 = (yuv_buffer *) malloc(sizeof(yuv_buffer));
	
	yuv420->y_height= h;	yuv420->uv_height	= h/2;
	yuv420->y_width	= w;	yuv420->uv_width	= w/2;
	yuv420->y_stride= w;	yuv420->uv_stride	= w/2;	
	
	yuv444->y_height = h;	yuv444->uv_height = h;
	yuv444->y_width = w;	yuv444->uv_width = w;
	yuv444->y_stride = w;	yuv444->uv_stride=w;

	yuv420->y = (unsigned char *)malloc(sizeof(unsigned char)*yuv420->y_height*yuv420->y_width);
	yuv420->u = (unsigned char *)malloc(sizeof(unsigned char)*yuv420->uv_height*yuv420->uv_width);
	yuv420->v = (unsigned char *)malloc(sizeof(unsigned char)*yuv420->uv_height*yuv420->uv_width);
	
	yuv444->y = (unsigned char *)malloc(sizeof(unsigned char)*yuv444->y_height*yuv444->y_width);
	yuv444->u = (unsigned char *)malloc(sizeof(unsigned char)*yuv444->uv_height*yuv444->uv_width);
	yuv444->v = (unsigned char *)malloc(sizeof(unsigned char)*yuv444->uv_height*yuv444->uv_width);
	
	for(i=0;i<h*w;i++){
		yuv444->y[i] = ( (  66 * rgb_in->r[i] + 129 * rgb_in->g[i] +  25 * rgb_in->b[i] + 128) >> 8) +  16;
		yuv444->u[i] = ( ( -38 * rgb_in->r[i] -  74 * rgb_in->g[i] + 112 * rgb_in->b[i] + 128) >> 8) + 128;
		yuv444->v[i] = ( ( 112 * rgb_in->r[i] -  94 * rgb_in->g[i] -  18 * rgb_in->b[i] + 128) >> 8) + 128;
	}

	etheora_444to420(yuv444,yuv420);

	return yuv420;
}

void initRGB(unsigned int w,unsigned int h){
	
	yuv444 = (yuv_buffer *)malloc(sizeof(yuv_buffer));

	yuv444->y_height = h;		yuv444->uv_height = h;
	yuv444->y_width = w;		yuv444->uv_width = w;
	yuv444->y_stride = w;		yuv444->uv_stride = w; 

	yuv444->y = (unsigned char *)malloc(sizeof(unsigned char)*yuv444->y_height*yuv444->y_width);
	yuv444->u = (unsigned char *)malloc(sizeof(unsigned char)*yuv444->uv_height*yuv444->uv_width);
	yuv444->v = (unsigned char *)malloc(sizeof(unsigned char)*yuv444->uv_height*yuv444->uv_width);
}

