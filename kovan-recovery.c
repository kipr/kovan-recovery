#include "log.h"
#include "framebuffer.h"
#include "flash_drive.h"
#include "image.h"

#include <unistd.h>

void scene_black(struct framebuffer *fb)
{
	struct pixel white = pixel_create(255, 255, 255);
	struct pixel black = pixel_create(0, 0, 0);
	char *recovery = "Recovery Mode";
	
	draw_string(recovery, 160 - string_width(recovery) / 2, 10, white, fb);
	framebuffer_fill(black, fb);
}

void scene_begin(struct framebuffer *fb)
{
	scene_black(fb);
	char *waiting = "Insert Flash Drive";
	struct pixel white = pixel_create(255, 255, 255);
	draw_string(waiting, 160 - string_width(waiting) / 2, 110, white, fb);
}

void scene_wait(struct framebuffer *fb)
{
	scene_black(fb);
	char *waiting = "Please Wait...";
	struct pixel white = pixel_create(255, 255, 255);
	draw_string(waiting, 160 - string_width(waiting) / 2, 110, white, fb);
}

void scene_loading(struct framebuffer *fb)
{
	scene_black(fb);
	char *waiting = "Please Wait...";
	struct pixel white = pixel_create(255, 255, 255);
	draw_string(waiting, 160 - string_width(waiting) / 2, 110, white, fb);
}

void scene_waiting(float pf, struct framebuffer *fb)
{
	scene_black(fb);
	char *str = "Updating...";
	struct pixel white = pixel_create(255, 255, 255);
	framebuffer_write_rect(10, 110, 300, 20, white, fb);
	framebuffer_fill_rect(12, 112, 297 * pf, 17, white, fb);
	draw_string(str, 160 - string_width(str) / 2, 140, white, fb);
}

void scene_done(struct framebuffer *fb)
{
	scene_black(fb);
	char *str = "Done! Restarting in 5 seconds...";
	struct pixel white = pixel_create(255, 255, 255);
	draw_string(str, 160 - string_width(str) / 2, 110, white, fb);
}

void scene_error(char *str, struct framebuffer *fb)
{
	scene_black(fb);
	struct pixel white = pixel_create(255, 255, 255);
	draw_string(str, 160 - string_width(str) / 2, 110, white, fb);
}

void msleep(unsigned long msec)
{
	usleep(msec * 1000L);
}

struct framebuffer *fb;

void progress(double fraction)
{
	fprintf(stdout, "%lf%% done.\n", fraction * 100.0);
	scene_waiting(fraction, fb);
	framebuffer_flip(fb);
}

void kovan_recovery()
{
	fb = framebuffer_open("/dev/fb0");
	
	scene_begin(fb);
	framebuffer_flip(fb);
	
	while(!flash_drive_mount()) {
		LOG_ERROR("Failed to mount flash drive.");
		msleep(1000);
	}
	
	LOG_NOTE("Mounted flash drive!");
	
	scene_wait(fb);
	framebuffer_flip(fb);
	msleep(500);
	
	if(flash_drive_update_type() == none) {
		scene_error("Couldn't find update! Exiting...", fb);
		framebuffer_flip(fb);
		msleep(1000);
		return;
	}
	
	scene_loading(fb);
	framebuffer_flip(fb);
	msleep(500);
	
	FILE *fptr = flash_drive_update();
	if(!fptr) {
		LOG_ERROR("Failed to open update file.");
		return;
	}
	
	if(!image_write(fptr, progress)) {
		scene_error("Image write failed! Exiting...", fb);
		framebuffer_flip(fb);
		msleep(1000);
		return;
	}
	
	fclose(fptr);
	flash_drive_umount();
	
	scene_done(fb);
	framebuffer_flip(fb);
	msleep(5000);
	
	scene_black(fb);
	framebuffer_flip(fb);
	
	framebuffer_close(fb);
}

int main(int argc, char *argv[])
{
	LOG_NOTE("kovan-recovery started");
	kovan_recovery();
	LOG_NOTE("kovan-recovery exiting");
	return 0;
}