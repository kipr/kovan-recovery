#ifndef _FLASH_DRIVE_H_
#define _FLASH_DRIVE_H_

#include "update_type.h"
#include <stdio.h>

int flash_drive_mount(void);
enum update_type flash_drive_update_type(void);
FILE *flash_drive_update(enum update_type type);
int flash_drive_umount(void);

#endif
