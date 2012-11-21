#include "flash_drive.h"
#include <sys/mount.h>

#define FLASH_DRIVE_UPDATE "/mnt/kovan_update.img"

int flash_drive_mount() {
	if(mount("/dev/sda1", "/mnt", "vfat", MS_NOATIME, NULL)) {
		PERROR("Mounting flash drive failed.");
		return 0;
	}
	NOTE("Mounted flash drive.");
	return 1;
}

enum update_type flash_drive_update_type()
{
	FILE *fptr = flash_drive_update();
	if(!fptr) return none;
	fclose(fptr);
	return image;
}

FILE *flash_drive_update()
{
	return fopen(FLASH_DRIVE_UPDATE, "r");
}

int flash_drive_umount()
{
	umount("/mnt")
}