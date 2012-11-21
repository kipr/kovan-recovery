#include "log.h"
#include "flash_drive.h"
#include "image.h"

void progress(double fraction)
{
	fprintf(stdout, "%lf%% done.\n", fraction * 100.0);
}

void kovan_recovery()
{
	if(!flash_drive_mount()) {
		LOG_ERROR("Failed to mount flash drive.");
		return;
	}
	if(flash_drive_update_type() == none) {
		LOG_ERROR("No update detected on flash drive.");
		return;
	}
	FILE *fptr = flash_drive_update();
	
	if(!fptr) {
		LOG_ERROR("Failed to open update file.");
		return;
	}
	image_write(fptr, progress);
	fclose(fptr);
	flash_drive_umount();
}

int main(int argc, char *argv[])
{
	LOG_NOTE("kovan-recovery started");
	kovan_recovery();
	LOG_NOTE("kovan-recovery exiting");
	return 0;
}