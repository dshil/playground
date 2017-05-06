#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>

static int more(FILE *fin, FILE *tty);
static size_t linesnum(FILE *tty);
static int tty_mode(FILE *tty, int mode);

static int nrows = 24;
static int ncols = 80;

int main(int ac, char *av[])
{
	FILE *tty = fopen("/dev/tty", "r");
	if (tty == NULL) {
		perror("/dev/tty");
		goto error;
	}

	struct winsize wbuf;
	if (ioctl(fileno(tty), TIOCGWINSZ, &wbuf) != -1) {
		nrows = wbuf.ws_row;
		ncols = wbuf.ws_col;
	}

	if (tty_mode(tty, 0) == -1)
		goto error;

	struct termios attr;
	if (tcgetattr(fileno(tty), &attr) == -1) {
		perror("tcgetattr");
		goto error;
	}

	attr.c_lflag &= ~ECHO;
	attr.c_lflag &= ~ICANON;
	attr.c_cc[VMIN] = 1;

	if (tcsetattr(fileno(tty), TCSANOW, &attr) == -1) {
		perror("tcsetattr");
		goto error;
	}

	FILE *fin = NULL;
	if (ac == 1) {
		if ((more(stdin, tty)) == -1) {
			perror("stdin");
			goto error;
		}
	} else {
		int i = 0;

		for (i = 1; i < ac; i++) {
			if (av[i] == NULL)
				continue;
			printf("==> %s <==\n", av[i]);

			fin = fopen(av[i], "r");
			if (fin == NULL) {
				fprintf(stderr, "fopen, err = ");
				perror(av[i]);
				goto error;
			}

			if (more(fin, tty) == -1)
				goto error;

			if (fclose(fin) == EOF) {
				fprintf(stderr, "fclose, err = ");
				perror(av[i]);
				goto error;
			}
		}
	}

	if (tty_mode(tty, 1) == -1)
		goto error;

	if (tty != NULL) {
		if (fclose(tty) == EOF) {
			fprintf(stderr, "fclose, err = ");
			perror("/dev/tty");
		}
	}

	exit(EXIT_SUCCESS);

error:
	tty_mode(tty, 1);

	if (fin != NULL) {
		if (fclose(fin) == EOF) {
			perror("fclose");
		}
	}

	if (tty != NULL) {
		if (fclose(tty) == EOF) {
			fprintf(stderr, "fclose, err = ");
			perror("/dev/tty");
		}
	}

	exit(EXIT_FAILURE);
}

static int more(FILE *fin, FILE *tty)
{
	int nl = 0; /* lines number */
	int c = 0; /* current character */
	int n = 0; /* lines to display determined by the user */

	for (;;) {
		c = getc(fin);
		if (ferror(fin) != 0) {
			perror("getc");
			return -1;
		}

		if (feof(fin))
			break;

		if (fputc(c, stdout) == EOF) {
			perror("fputs");
			return -1;
		}

		if (c == '\n') {
			nl++;

			if (nl == nrows) {
				n = linesnum(tty);
				if (n == -1)
					return -1;
				else if (n == 0)
					break;
				else
					nl -= n;
			}
		}
	}

	return 0;
}

static size_t linesnum(FILE *tty)
{
	fprintf(stderr, "\033[7m more? \033[m\n");
	int c = 0;

	for (;;) {
		c = getc(tty);
		if ((ferror(tty)) != 0)
			return -1;

		if (c == 'q') {
			return 0;
		} else if (c == ' ') {
			return nrows;
		} else if (c == '\n') {
			return 1;
		}
	}
	return 0;
}

static int tty_mode(FILE *tty, int mode)
{
	static struct termios orig_mode;
	static int orig_flags;
	if (mode == 0) {
		if (tcgetattr(fileno(tty), &orig_mode) == -1) {
			perror("tcgetattr");
			return -1;
		}

		if ((orig_flags = fcntl(fileno(tty), F_GETFL)) == -1) {
			perror("fcntl");
			return -1;
		}
	} else {
		if (tcsetattr(fileno(tty), TCSANOW, &orig_mode) == -1) {
			perror("tcsetattr");
			return -1;
		}

		if (fcntl(fileno(tty), F_SETFL, orig_flags) == -1) {
			perror("fcntl");
			return -1;
		}
	}

	return 0;
}
