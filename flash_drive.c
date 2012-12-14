#include "flash_drive.h"
#include "log.h"
#include <sys/mount.h>

#define FLASH_DRIVE_UPDATE "/mnt/kovan_update.img"
#define FLASH_DRIVE_COMPRESSED_UPDATE "/mnt/kovan_update.img.gz"

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

enum update_type flash_drive_update_type()
{
	FILE *fptr = flash_drive_update(image);
	if(fptr) {
		fclose(fptr);
		return image;
	}
	fptr = flash_drive_update(compressed_image);
	if(fptr) {
		fclose(fptr);
		return compressed_image;
	}
	return none;
}

FILE *flash_drive_update(enum update_type type)
{
	switch(type) {
	case image:
		return fopen(FLASH_DRIVE_UPDATE, "r");
	case compressed_image:
		return fopen(FLASH_DRIVE_COMPRESSED_UPDATE, "r");
	default: break;
	}
	return 0;
}

int flash_drive_umount(void)
{
#ifdef __linux__
	umount("/mnt");
#endif
	return 1;
}