#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>

static int more(FILE *fin, FILE *tty);
static size_t linesnum(FILE *tty);
static int tty_mode(FILE *tty, int mode);
static int set_signal_handler(void);
static void set_term_winsz(void);
static void handle_sig(int signum);
static int set_term_settings();

static int nrows = 24;
static int ncols = 80;
static FILE *tty = NULL;

int main(int ac, char *av[])
{
	if (set_signal_handler() == -1)
		exit(EXIT_FAILURE);

	tty = fopen("/dev/tty", "r");
	if (tty == NULL) {
		perror("/dev/tty");
		goto error;
	}

	set_term_winsz();
	if (set_term_settings() == -1)
		goto error;

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

	if (fin != NULL)
		if (fclose(fin) == EOF)
			perror("fclose");

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

static int set_term_settings()
{
	if (tty_mode(tty, 0) == -1)
		return -1;

	struct termios attr;
	if (tcgetattr(fileno(tty), &attr) == -1) {
		perror("tcgetattr");
		return -1;
	}

	attr.c_lflag &= ~ECHO;
	attr.c_lflag &= ~ICANON;

	if (tcsetattr(fileno(tty), TCSANOW, &attr) == -1) {
		perror("tcsetattr");
		return -1;
	}
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

static int set_signal_handler(void)
{
	int sigs[] = {SIGINT, SIGWINCH, SIGCONT};
	const int siglen = sizeof(sigs)/sizeof(int);

	for (int i = 0; i < siglen; i++) {
		errno = 0;
		signal(sigs[i], handle_sig);
		if (errno) {
			perror("signal");
			return -1;
		}
	}
	return 0;
}

static void handle_sig(int signum)
{
	switch (signum) {
		case SIGINT:
			// tcsh and bash will reset the terminal settings to default
			// if the process will be interrupted by the signal, but let's not
			// relay on this.
			if (tty_mode(tty, 1) == -1)
				exit(EXIT_FAILURE);
			exit(EXIT_SUCCESS);
		case SIGWINCH:
			set_term_winsz();
			break;
		case SIGCONT:
			if (set_term_settings() == -1)
				exit(EXIT_FAILURE);
			break;
	}
}

static void set_term_winsz(void)
{
	struct winsize wbuf;
	if (ioctl(fileno(tty), TIOCGWINSZ, &wbuf) != -1) {
		nrows = wbuf.ws_row;
		ncols = wbuf.ws_col;
	}
}
