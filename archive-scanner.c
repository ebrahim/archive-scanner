#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <magic.h>

#define DEFAULT_RESULT "data"

const int buff_size = 1<<20;
const int window_size = 64;

int
main(int argc, char* argv[])
{
	int fd = 0;
	magic_t magic = magic_open(MAGIC_CHECK | MAGIC_NO_CHECK_ASCII | MAGIC_NO_CHECK_COMPRESS);

	if (argc >= 2 && *argv[1] && strcmp(argv[1], "-"))
	{
		fd = open(argv[1], O_RDONLY);
		if (fd < 0)
		{
			fprintf(stderr, "Error: Unable to open specified file\n");
			return 1; 
		}
		if (argc >= 3 && *argv[2])
		{
			if (magic_compile(magic, argv[2]) < 0)
			{
				fprintf(stderr, "Magic Compile Error: %s\n", magic_error(magic));
				return 1;
			}
			if (magic_load(magic, argv[2]) < 0)
			{
				fprintf(stderr, "Magic Load Error: %s\n", magic_error(magic));
				return 1;
			}
		}
		else
			magic_load(magic, NULL);
	}


	int read_offset = 0;
	int offset = 0;
	for (;;)
	{
		char buff[buff_size];
		int len = read(fd, buff + read_offset, buff_size - read_offset);
		char* buff_end = buff + read_offset + len;
		char* buff_ptr;
		for (buff_ptr = buff; buff_ptr + window_size <= buff_end; ++buff_ptr, ++offset)
		{
			const char* res = magic_buffer(magic, buff_ptr, window_size);
			if (strcmp(res, DEFAULT_RESULT))
				printf("%d: %s\n", offset, res);
		}
		if (len <= 0)
			break;
		read_offset = window_size - 1;
		memcpy(buff, buff_end - read_offset, read_offset);
	}

	magic_close(magic);
	close(fd);

	return 0;
}
