#ifndef _FLASH_DRIVE_H_
#define _FLASH_DRIVE_H_

#include "update_type.h"

int flash_drive_mount();
enum update_type flash_drive_update_type();
FILE *flash_drive_update();
int flash_drive_unmount();

#endif
