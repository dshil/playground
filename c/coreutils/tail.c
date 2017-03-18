#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include "reader.h"

/* TODO:
 * [] Add -f option.
 * [] Handle signals gracefully.
 */

static int read_tail_lines(FILE *f, char *filename);
static int read_tail_bytes(FILE *f, char *filename);
static int read_tail_blocks(FILE *f, char *filename);

static int read_stdin_tail(struct read_config *conf);
static void usage(char *prog_name);
static int listen_file_changes(char *filename);

static int nlines = 10;
static int nbytes = -1;
static int nblocks = -1;

int main(int ac, char *av[])
{
	char *nlineval = NULL;
	char *nbyteval = NULL;
	char *nblockval = NULL;
	int suppress_file_name = 0;

	char *observe_file = NULL;

	int opt = 0;
	while((opt = getopt(ac, av, "qf:b:c:n:")) != -1) {
		switch(opt) {
			case 'q': suppress_file_name = 1; break;
			case 'f': observe_file = optarg; break;
			case 'b': nblockval = optarg; break;
			case 'n': nlineval = optarg; break;
			case 'c': nbyteval = optarg; break;
			default:
				  usage(av[0]);
				  exit(EXIT_FAILURE);
		}
	}

	const int line_byte = (nlineval != NULL) && (nbyteval != NULL);
	const int line_block = (nlineval != NULL) && (nblockval != NULL);
	const int byte_block = (nbyteval != NULL) && (nblockval != NULL);

	if (line_byte || line_block || byte_block) {
		usage(av[0]);
		exit(EXIT_FAILURE);
	}

	if (parse_num(nlineval, &nlines) == -1)
		exit(EXIT_FAILURE);

	if (parse_num(nbyteval, &nbytes) == -1)
		exit(EXIT_FAILURE);

	if (parse_num(nblockval, &nblocks) == -1)
		exit(EXIT_FAILURE);

	struct read_config config;

	if (nblocks != -1)
		config.read_file = read_tail_blocks;
	else if (nbytes != -1)
		config.read_file = read_tail_bytes;
	else
		config.read_file = read_tail_lines;

	if (ac == optind) {
		if (read_stdin_tail(&config) == -1)
			exit(EXIT_FAILURE);

	} else {
		config.is_print = ((ac - optind) > 1) && !suppress_file_name;
		config.argv = av;
		config.ac = ac;

		if (read_files(&config) == -1) {
			exit(EXIT_FAILURE);
		}
	}

	if (observe_file != NULL) {
		if (listen_file_changes(observe_file) == -1)
			exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

static int read_stdin_tail(struct read_config *conf)
{
	// std is not seekable, need to read the whole stuff from stdin to the
	// temporary file.
	FILE *tmp = NULL;
	const int buf_size = BUFSIZ*4;
	char buf[buf_size];
	ssize_t n = 0;

	if ((tmp = tmpfile()) == NULL) {
		perror("temporary file");
		goto error;
	}

	while((n = fread(buf, 1, buf_size, stdin)) > 0) {
		if (fwrite(buf, 1, n, tmp) != n) {
			perror("temporary file");
			goto error;
		}
	}

	if (conf->read_file(tmp, "stdin") == -1)
		goto error;

	if (fclose(tmp) == -1) {
		perror("temporary file");
		goto error;
	}

	return 0;

error:
	if (tmp != NULL) {
		if (fclose(tmp) == -1) {
			perror("temporary file");
		}
	}
	return -1;
}

static int read_tail_lines(FILE *f, char *filename)
{
	int c = 0;
	int n = 0;
	int nc = 0; /* number of characters to print */

	if (fseek(f, 0, SEEK_END) == -1) {
		perror(filename);
		return -1;
	}

	do {
		if (fseek(f, -1, SEEK_CUR) == -1) {
			// the beginning of the file was reached, it's time to break
			break;
		}

		c = getc(f);
		if (ferror(f) != 0) {
			perror(filename);
			return -1;
		}

		if (c == '\n' && (++n == (nlines + 1)))
			continue;

		nc++;
		if (ungetc(c, f) == EOF) {
			perror(filename);
			return -1;
		}

	} while (n != (nlines+1));

	return read_and_print_bytes(f,filename, nc);
}

static int read_tail_bytes(FILE *f, char *filename)
{
	if (fseek(f, 0, SEEK_END) == -1) {
		perror(filename);
		return -1;
	}

	const int offset = ftell(f);
	if (offset == -1) {
		perror(filename);
		return -1;
	}

	if (offset == 0)
		return 0;

	int bytes = 0;
	if (nbytes > offset) {
		if (fseek(f, 0, SEEK_SET) == -1) {
			perror(filename);
			return -1;
		}
		return read_and_print_bytes(f, filename, offset);
	}

	if (fseek(f, -nbytes, SEEK_END) == -1) {
		perror(filename);
		return -1;
	}

	return read_and_print_bytes(f, filename, nbytes);
}

static int read_tail_blocks(FILE *f, char *filename)
{
	if (fseek(f, 0, SEEK_END) == -1) {
		perror(filename);
		return -1;
	}

	// 512, 256, 128, ..., 1.
	char block_sizes[9];
	const int len = sizeof(block_sizes)/sizeof(block_sizes[0]);

	int i = 0;
	int block_size = 512;
	for (;;) {
		// successfully read the required number of 512 bytes blocks.
		if (block_size == 512 && (block_sizes[i] == nblocks))
			break;

		if (fseek(f, -block_size, SEEK_CUR) == -1) {
			// Try a block with the less size. If the block size equals to 1,
			// it's time to break.
			if (++i == len)
				break;

			block_size /= 2;
			continue;
		}
		block_sizes[i]++;
	}

	i = 0;
	block_size = 512;
	int size = 0;

	for (i = 0; i < len; i++) {
		size += block_size * block_sizes[i];
		block_size /= 2;
	}

	return read_and_print_bytes(f, filename, size);
}

static void usage(char *prog_name)
{
	fprintf(stderr,
			"Usage: %s [-q] [-f file] [-b blocks | -c bytes | -n lines] [file ...]\n",
			prog_name);
}

static int listen_file_changes(char *filename)
{
	ssize_t fd = 0;
	if ((fd = inotify_init()) == -1) {
		perror(filename);
		return -1;
	}

	if (close(fd) == -1) {
		perror(filename);
		return -1;
	}

	return 0;
}
