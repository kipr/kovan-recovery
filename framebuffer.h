#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <linux/fb.h>

struct framebuffer
{
	int fd;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	unsigned char *display;
	unsigned char *display_back;
};

struct pixel
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct pixel pixel_create(unsigned char r, unsigned char g, unsigned char b);

struct framebuffer *framebuffer_open(const char *dev);
void framebuffer_write_pixel(unsigned short x, unsigned short y, struct pixel p, struct framebuffer *fb);
void framebuffer_write_rect(unsigned short x, unsigned short y, unsigned short w, unsigned short h, struct pixel p, struct framebuffer *fb);
void framebuffer_fill_rect(unsigned short x, unsigned short y, unsigned short w, unsigned short h, struct pixel p, struct framebuffer *fb);
void framebuffer_fill(struct pixel p, struct framebuffer *fb);
void framebuffer_flip(struct framebuffer *fb);
void framebuffer_close();

#endif
