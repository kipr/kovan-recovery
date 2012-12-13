#include "framebuffer.h"
#include "consolas_16.h"

#ifdef __linux__
#include <linux/fb.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>

struct pixel pixel_create(unsigned char r, unsigned char g, unsigned char b)
{
	struct pixel ret;
	ret.r = r;
	ret.g = g;
	ret.b = b;
	return ret;
}

static unsigned char fontpixels[STB_SOMEFONT_BITMAP_HEIGHT][STB_SOMEFONT_BITMAP_WIDTH];
static stb_fontchar fontdata[STB_SOMEFONT_NUM_CHARS];

unsigned int string_width(char *str)
{
	unsigned int x = 0;
	while (*str) {
		x += fontdata[(*str++) - STB_SOMEFONT_FIRST_CHAR].advance_int;
	}
	return x;
}

void draw_string(char *str, int x, int y, struct pixel p, struct framebuffer *fb)
{
	while (*str) {
		int char_codepoint = *str++;
		stb_fontchar *cd = &fontdata[char_codepoint - STB_SOMEFONT_FIRST_CHAR];
		/* printf("%c (%d, %d) to (%d, %d), (%d, %d) to (%d, %d)\n", char_codepoint,
			(int)(cd->s0 * STB_SOMEFONT_BITMAP_WIDTH), (int)(cd->t0 * STB_SOMEFONT_BITMAP_HEIGHT),
			(int)(cd->s1 * STB_SOMEFONT_BITMAP_WIDTH), (int)(cd->t1 * STB_SOMEFONT_BITMAP_HEIGHT),
			cd->x0, cd->y0, cd->x1, cd->y1); */
		
		const unsigned int sx = cd->s0 * STB_SOMEFONT_BITMAP_WIDTH;
		const unsigned int sy = cd->t0 * STB_SOMEFONT_BITMAP_HEIGHT;
		const unsigned int ex = cd->s1 * STB_SOMEFONT_BITMAP_WIDTH;
		const unsigned int ey = cd->t1 * STB_SOMEFONT_BITMAP_HEIGHT;
		
		unsigned int cx = sx;
		unsigned int px = 0;
		for(; cx < ex; ++cx, ++px) {
			unsigned int cy = sy;
			unsigned int py = 0;
			for(; cy < ey; ++cy, ++py) {
				const unsigned int l = fontpixels[cy][cx];
				const float lf = l / 255.0;
				// printf("\tValue at (%d, %d) = %d\n", cx, cy, l);
				struct pixel mod = pixel_create(lf * p.r, lf * p.g, lf * p.b);
				// printf("%d, %d, %d\n", mod.r, mod.g, mod.b);
				framebuffer_write_pixel(x + px + cd->x0, y + py + cd->y0, mod, fb);
			}
		}
		x += cd->advance_int;
	}
}

struct framebuffer *framebuffer_open(const char *dev)
{
#ifdef __linux__
	struct framebuffer *ret = (struct framebuffer *)malloc(sizeof(struct framebuffer));
	
	ret->fd = open(dev, O_RDWR);
	printf("The framebuffer device was opened successfully.\n");
	
	// Get fixed screen information
	if(ioctl(ret->fd, FBIOGET_FSCREENINFO, &ret->finfo) == -1) {
		perror("Error reading fixed information");
		return 0;
	}

	// Get variable screen information
	if(ioctl(ret->fd, FBIOGET_VSCREENINFO, &ret->vinfo) == -1) {
		perror("Error reading variable information");
		return 0;
	}
		
	const unsigned long screensize = ret->vinfo.xres * ret->vinfo.yres * ret->vinfo.bits_per_pixel / 8;
	ret->display = (unsigned char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, ret->fd, 0);
	ret->display_back = (unsigned char *)malloc(screensize);
	
	STB_SOMEFONT_CREATE(fontdata, fontpixels, STB_SOMEFONT_BITMAP_HEIGHT);
	
	return ret;
#else
	return 0;
#endif
}

void framebuffer_write_pixel(unsigned short x, unsigned short y, struct pixel p, struct framebuffer *fb)
{
	if(x < 0 || y < 0 || x >= fb->vinfo.xres || y >= fb->vinfo.yres) return;
	
	unsigned long location = (x + fb->vinfo.xoffset) * (fb->vinfo.bits_per_pixel / 8)
		+ (y + fb->vinfo.yoffset) * fb->finfo.line_length;
	if(fb->vinfo.bits_per_pixel == 32) {
		*(fb->display_back + location) = p.b;
		*(fb->display_back + location + 1) = p.g;
		*(fb->display_back + location + 2) = p.r;
		*(fb->display_back + location + 3) = 0;
	} else {
		const unsigned short t = (p.r >> 3) << 11 | (p.g >> 2) << 5 | (p.b >> 3);
		*((unsigned short *)(fb->display_back + location)) = t;
	}
}

void framebuffer_write_rect(unsigned short x, unsigned short y, unsigned short w, unsigned short h, struct pixel p, struct framebuffer *fb)
{
	unsigned int i = x;
	for(; i <= x + w; ++i) {
		framebuffer_write_pixel(i, y, p, fb);
	}
	for(i = x; i <= x + w; ++i) {
		framebuffer_write_pixel(i, y + h, p, fb);
	}
	unsigned int j = y;
	for(; j <= y + h; ++j) {
		framebuffer_write_pixel(x, j, p, fb);
	}
	for(j = y; j <= y + h; ++j) {
		framebuffer_write_pixel(x + w, j, p, fb);
	}
}

void framebuffer_fill_rect(unsigned short x, unsigned short y, unsigned short w, unsigned short h, struct pixel p, struct framebuffer *fb)
{
	unsigned int i = x;
	for(; i < x + w; ++i) {
		unsigned int j = y;
		for(; j < y + h; ++j) {
			framebuffer_write_pixel(i, j, p, fb);
		}
	}
}

void framebuffer_fill(struct pixel p, struct framebuffer *fb)
{
	framebuffer_fill_rect(0, 0, fb->vinfo.xres, fb->vinfo.yres, p, fb);
}

void framebuffer_flip(struct framebuffer *fb)
{
	const unsigned long screensize = fb->vinfo.xres * fb->vinfo.yres * fb->vinfo.bits_per_pixel / 8;
	memcpy(fb->display, fb->display_back, screensize);
}

void framebuffer_close(struct framebuffer *fb)
{
	const unsigned long screensize = fb->vinfo.xres * fb->vinfo.yres * fb->vinfo.bits_per_pixel / 8;
	free(fb->display_back);
	munmap(fb->display, screensize);
	close(fb->fd);
	free(fb);
}