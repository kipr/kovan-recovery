#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdio.h>

#include "progress.h"

int image_write(FILE *in, struct progress *reporter);

#endif
