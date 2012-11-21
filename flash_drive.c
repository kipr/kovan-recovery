#include "flash_drive.h"
#include <sys/mount.h>

#define FLASH_DRIVE_UPDATE "/mnt/kovan_update.img"

int flash_drive_mount(void)
{
#ifdef __linux__
	if(mount("/dev/sda1", "/mnt", "vfat", MS_NOATIME, NULL)) {
		LOG_PERROR("Mounting flash drive failed.");
		return 0;
	}
	LOG_NOTE("Mounted flash drive.");
#endif
	return 1;
}

enum update_type flash_drive_update_type(void)
{
	FILE *fptr = flash_drive_update();
	if(!fptr) return none;
	fclose(fptr);
	return image;
}

FILE *flash_drive_update(void)
{
	return fopen(FLASH_DRIVE_UPDATE, "r");
}

int flash_drive_umount(void)
{
#ifdef __linux__
	umount("/mnt")
#endif
	return 1;
}