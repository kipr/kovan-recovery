#include "image.h"

#include "log.h"

#define BLOCK_SIZE (1048576) // 1 MB

int image_write(FILE *in, struct progress *reporter)
{
	FILE *out = fopen("/dev/mmcblk0", "w");
	char buffer[BLOCK_SIZE];
	size_t read = 0;
	size_t total = 0;
	while((read = fread(buffer, sizeof(char), BLOCK_SIZE, in)) > 0) {
		fwrite(buffer, sizeof(char), read, out);
		if(ferror(out)) {
			LOG_PERROR("Writing block failed");
			return 0;
		}
		total += read;
	}
	if(ferror(in)) {
		LOG_PERROR("Reading block failed");
		return 0;
	}
	LOG_NOTE("Copied %u bytes to internal memory card.", total);
	fclose(out);
	return 1;
}