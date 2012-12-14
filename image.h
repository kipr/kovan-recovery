#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdio.h>

int image_write(FILE *in, void (*progress_callback)(double));
int image_write_compressed(FILE *in, void (*progress_callback)(double));

#endif
