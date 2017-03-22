#include <poll.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/inotify.h>
#include "reader.h"

/* TODO:
 * [] Handle signals gracefully.
 */

static int read_tail_lines(FILE *f);
static int read_tail_bytes(FILE *f);
static int read_tail_blocks(FILE *f);

static int read_stdin_tail(struct read_config *conf);
static void usage(char *prog_name);
static int listen_file_changes(char *filename);
static int handle_inotify_events(int ifd, char *filename);

static int write_from_path(char *filepath);
static int write_from_to(FILE *src, FILE *dst);

static int nlines = 10;
static int nbytes = -1;
static int nblocks = -1;

int main(int ac, char *av[])
{
	char *nlineval = NULL;
	char *nbyteval = NULL;
	char *nblockval = NULL;
	int suppress_file_name = 0;

	int is_following_file = 0;
	char *following_file = NULL;

	int opt = 0;
	while((opt = getopt(ac, av, "qf:b:c:n:")) != -1) {
		switch(opt) {
			case 'q': suppress_file_name = 1; break;
			case 'f': following_file = optarg; break;
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

	if (ac == optind && following_file == NULL) {
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

	if (following_file != NULL) {
		if (write_from_path(following_file) == -1)
			exit(EXIT_FAILURE);

		if (listen_file_changes(following_file) == -1)
			exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

static int read_stdin_tail(struct read_config *conf)
{
	// std is not seekable, need to read the whole stuff from stdin to the
	// temporary file.
	FILE *tmp = tmpfile();
	if (tmp == NULL) {
		perror("tmpfile");
		goto error;
	}

	if (write_from_to(stdin, tmp) == -1)
		goto error;

	if (conf->read_file(tmp) == -1)
		goto error;

	if (fclose(tmp) == -1) {
		perror("fclose");
		goto error;
	}

	return 0;

error:
	if (tmp != NULL) {
		if (fclose(tmp) == -1) {
			perror("fclose");
		}
	}
	return -1;
}

static int read_tail_lines(FILE *f)
{
	int c = 0;
	int n = 0;
	int nc = 0; /* number of characters to print */

	if (fseek(f, 0, SEEK_END) == -1) {
		perror("fseek");
		return -1;
	}

	do {
		if (fseek(f, -1, SEEK_CUR) == -1) {
			// the beginning of the file was reached, it's time to break
			break;
		}

		c = getc(f);
		if (ferror(f) != 0) {
			perror("getc");
			return -1;
		}

		if (c == '\n' && (++n == (nlines + 1)))
			continue;

		nc++;
		if (ungetc(c, f) == EOF) {
			perror("ungetc");
			return -1;
		}

	} while (n != (nlines+1));

	return read_and_print_bytes(f, nc);
}

static int read_tail_bytes(FILE *f)
{
	if (fseek(f, 0, SEEK_END) == -1) {
		perror("fseek");
		return -1;
	}

	const int offset = ftell(f);
	if (offset == -1) {
		perror("ftell");
		return -1;
	}

	if (offset == 0)
		return 0;

	int bytes = 0;
	if (nbytes > offset) {
		if (fseek(f, 0, SEEK_SET) == -1) {
			perror("fseek");
			return -1;
		}
		return read_and_print_bytes(f, offset);
	}

	if (fseek(f, -nbytes, SEEK_END) == -1) {
		perror("fseek");
		return -1;
	}

	return read_and_print_bytes(f, nbytes);
}

static int read_tail_blocks(FILE *f)
{
	if (fseek(f, 0, SEEK_END) == -1) {
		perror("fseek");
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

	return read_and_print_bytes(f, size);
}

static void usage(char *prog_name)
{
	fprintf(stderr,
			"Usage: %s [-q] [-f file] [-b blocks | -c bytes | -n lines] [file ...]\n",
			prog_name);
}

static int listen_file_changes(char *filename)
{
	ssize_t ifd = 0;
	ssize_t wfd = 0;

	/* Create the file descriptor for accessing the inotify API */
	if ((ifd = inotify_init1(IN_NONBLOCK)) == -1) {
		perror("inotify_init1");
		goto error;
	}

	/* Add a new watch for the filename */
	if ((wfd = inotify_add_watch(ifd, filename, IN_MODIFY)) == -1) {
		perror("inotify_add_watch");
		goto error;
	}

	/* Prepare for polling */
	nfds_t nfds = 2; /* number of file descriptors for polling */
	struct pollfd fds[2];

	/* Console input */
	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;

	/* Inotify input */
	fds[1].fd = ifd;
	fds[1].events = POLLIN;

	int poll_num = 0;
	char buf = 0;

	/* Wait for inotify events and/or terminal input */
	while(1) {
		if ((poll_num = poll(fds, nfds, -1)) == -1) {
			perror("poll");
			return -1;
		}

		if (poll_num > 0) {
			/* Console input is available. Empty stdin and continue. */
			if (fds[0].revents & POLLIN) {
				while (read(STDIN_FILENO, &buf, 1) > 0)
					continue;
			}

			if (fds[1].revents & POLLIN) {
				if (handle_inotify_events(ifd, filename) == -1)
					goto error;
			}
		}
	}

	/* Remove the watch from an inotify watch list */
	if (inotify_rm_watch(ifd, wfd) == -1) {
		perror("inotify_rm_watch");
		goto error;
	}

	if (close(wfd) == -1) {
		perror("close");
		goto error;
	}

	if (close(ifd) == -1) {
		perror("close");
		goto error;
	}

	return 0;

error:
	if (ifd != -1) {
		if (close(ifd) == -1) {
			perror("close");
		}
	}

	if (wfd != -1) {
		if (close(wfd) == -1) {
			perror("close");
		}
	}

	return -1;
}

static int handle_inotify_events(int ifd, char *filename)
{
	/* Some systems can't read integer variables if they are not
	   properly aligned. On other systems, incorrect alignment may
	   decrease performance. The buffer used for reading from the
	   inotify file descriptor should have the same alignment as
	   struct inotify_event.
	*/
	char buf[4*BUFSIZ]
		__attribute__ ((aligned(__alignof__(struct inotify_event))));

	const int buf_len = sizeof(buf)/sizeof(buf[0]);

	const struct inotify_event *event;
	char *ptr = NULL;
	ssize_t len = 0;
	int is_truncated = 0;

	errno = 0;

	for (;;) {
		len = read(ifd, buf, buf_len);
		if (len == -1 && errno != EAGAIN) {
			perror("read");
			return -1;
		}

		/* If the nonblocking read() found no events to read, then
		   it returns -1 with errno set to EAGAIN. Exit the loop.
		*/

		if (len <= 0)
			return 0;

		/* Iterate over all events in the buffer */
		for (ptr = buf; ptr < buf + len;
				ptr += sizeof(struct inotify_event) + event->len) {

			event = (const struct inotify_event *) ptr;

			if (event->mask & IN_MODIFY) {
				if (write_from_path(filename) == -1)
					return -1;
			}
		}
	}
}

static int write_from_path(char *filepath)
{
	FILE *f = fopen(filepath, "r");
	if (f == NULL) {
		perror("fopen");
		goto error;
	}

	if (write_from_to(f, stdout) == -1)
		goto error;

	if (fclose(f) == -1) {
		perror("fclose");
		return -1;
	}

	return 0;

error:
	if (f != NULL) {
		if (fclose(f) == -1)
			perror("fclose");
	}
	return -1;
}

static int write_from_to(FILE *src, FILE *dst)
{
	char buf[BUFSIZ*4];
	const int buf_len = BUFSIZ*4;
	ssize_t n = 0;

	while ((n = fread(buf, 1, buf_len, src)) > 0) {
		if (fwrite(buf, 1, n, dst) != n) {
			perror("fwrite");
			return -1;
		}
	}

	return 0;
}
