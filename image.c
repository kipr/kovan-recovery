#include "image.h"

#include "log.h"

#define BLOCK_SIZE (1048576) // 1 MB

int write_image(FILE *in, struct progress *reporter)
{
	FILE *out = fopen("/dev/mmcblk0");
	char buffer[BLOCK_SIZE];
	size_t read = 0;
	size_t total = 0;
	while((read = fread(buffer, sizeof(char), BLOCK_SIZE, in)) > 0) {
		fwrite(buffer, read, out);
		if(ferror(out)) {
			PERROR("Writing block failed");
			return 0;
		}
		total += read;
	}
	if(ferror(in)) {
		PERROR("Reading block failed");
		return 0;
	}
	NOTE("Copied %lu bytes to internal memory card.", total);
	fclose(out);
	return 1;
}