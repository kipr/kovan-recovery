#include "image.h"

#include "log.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BLOCK_SIZE (1048576) // 1 MB

int image_write(FILE *in, void (*progress_callback)(double))
{
	struct stat in_stat;
	if(!fstat(fileno(in), &in_stat)) {
		LOG_PERROR("image fstat failed.");
		return 0;
	}
	off_t size = in_stat.st_size;
	
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
		
		// Report how far we are to progress callback
		if(progress_callback) progress_callback((double)total / (double)size);
	}
	if(ferror(in)) {
		LOG_PERROR("Reading block failed");
		return 0;
	}
	LOG_NOTE("Copied %u bytes to internal memory card.", (unsigned int)total);
	fclose(out);
	return 1;
}