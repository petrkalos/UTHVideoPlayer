typedef struct yuv_buffer{
	unsigned char *y;
	unsigned char *u;
	unsigned char *v;
	int y_width;
	int y_height;
	int uv_width;
	int uv_height;
	int y_stride;
	int uv_stride;
}yuv_buffer;

typedef struct rgb{
	unsigned char *r;
	unsigned char *g;
	unsigned char *b;
	unsigned int w;
	unsigned int h;
}rgb;

void yuv420torgb_8(rgb *rgb_out,yuv_buffer *yuv);
yuv_buffer *rgbtoyuv420_8(rgb *rgb_in);

void initRGB(unsigned int w, unsigned int h);

rgb *yuv420torgb_4(yuv_buffer *yuv,int dither);
yuv_buffer *rgbtoyuv420_4(rgb *rgb_in);

int getFrame(rgb *rgb_out,unsigned int frame);
void initFrame(int argc,char *argv[]);
