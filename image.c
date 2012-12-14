#include "image.h"

#include "log.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>
#include <assert.h>

#define BLOCK_SIZE (1048576) // 1 MB

int image_write(FILE *in, void (*progress_callback)(double))
{
	struct stat in_stat;
	if(fstat(fileno(in), &in_stat)) {
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

int image_write_compressed(FILE *in, void (*progress_callback)(double))
{
	struct stat in_stat;
	if(fstat(fileno(in), &in_stat)) {
		LOG_PERROR("image fstat failed.");
		return 0;
	}
	off_t size = in_stat.st_size;

	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = 0;
	stream.next_in = Z_NULL;
	if (inflateInit2(&stream, 16 + MAX_WBITS) != Z_OK) {
		LOG_ERROR("inflateInit2 failed.");
		return 0;
	}
	
	FILE *out = fopen("/dev/mmcblk0", "w");
	if(!out) {
		LOG_PERROR("Couldn't open memory card device for writing.");
		return 0;
	}
	unsigned char buffer[BLOCK_SIZE];
	unsigned char outbuff[BLOCK_SIZE];
	size_t read = 0;
	size_t total = 0;
	while((read = fread(buffer, sizeof(char), BLOCK_SIZE, in)) > 0) {
		stream.avail_in = read;
		if(ferror(in)) {
			inflateEnd(&stream);
			return Z_ERRNO;
		}
		if(stream.avail_in == 0) break;
		stream.next_in = buffer;

		do {
			stream.avail_out = BLOCK_SIZE;
			stream.next_out = outbuff;
			int ret = inflate(&stream, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);
			switch (ret) {
			case Z_NEED_DICT:
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				LOG_ERROR("zlib error %d", ret);
				inflateEnd(&stream);
				return 0;
			}
			unsigned have = BLOCK_SIZE - stream.avail_out;
			if(fwrite(outbuff, 1, have, out) != have || ferror(out)) {
				LOG_PERROR("Writing block failed");
				inflateEnd(&stream);
				return 0;
			}
		} while(stream.avail_out == 0);
		total += read;

		// Report how far we are to progress callback
		if(progress_callback) progress_callback((double)total / (double)size);
	}
	
	fclose(out);

	if(ferror(in)) {
		LOG_PERROR("Reading block failed");
		return 0;
	}
	
	LOG_NOTE("Wrote image (%u)", (unsigned int)total);
	
	return 1;
}