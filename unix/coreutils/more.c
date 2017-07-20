#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>

static int more(FILE *fin, FILE *tty, sigset_t *sigs);
static size_t linesnum(FILE *tty, int rows);
static int tty_mode(int mode);
static int set_signal_handler(void);
static void set_term_winsz(void);
static void handle_sig(int signum);
static int set_term_settings(void);

static sig_atomic_t nrows = 24;
static sig_atomic_t ncols = 80;
static int tty_fileno = 0;

int main(int ac, char *av[])
{
	FILE *tty = fopen("/dev/tty", "r");
	if (tty == NULL) {
		perror("/dev/tty");
		goto error;
	}
	tty_fileno = fileno(tty);

	set_term_winsz();
	if (set_term_settings() == -1) {
		fprintf(stderr, "set_term_settings: %s\n", strerror(errno));
		goto error;
	}

	if (set_signal_handler() == -1)
		exit(EXIT_FAILURE);

	sigset_t sigs;
	if (sigemptyset(&sigs) == -1) {
		perror("sigemptyset");
		return -1;
	}

	if (sigaddset(&sigs, SIGWINCH) == -1) {
		perror("sigaddset");
		return -1;
	}

	FILE *fin = NULL;
	if (ac == 1) {
		if ((more(stdin, tty, &sigs)) == -1) {
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

			if (more(fin, tty, &sigs) == -1)
				goto error;

			if (fclose(fin) == EOF) {
				fprintf(stderr, "fclose, err = ");
				perror(av[i]);
				goto error;
			}
		}
	}

	if (tty_mode(1) == -1)
		goto error;

	if (tty != NULL) {
		if (fclose(tty) == EOF) {
			fprintf(stderr, "fclose, err = ");
			perror("/dev/tty");
		}
	}

	exit(EXIT_SUCCESS);

error:
	tty_mode(1);

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

static int more(FILE *fin, FILE *tty, sigset_t *sigs)
{
	int nl = 0; /* lines number */
	int c = 0; /* current character */
	int n = 0; /* lines to display determined by the user */

	// Copy of actual rows number to prevent a race during
	// a signal handling.
	int cp_rows = 0;

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

			if (sigprocmask(SIG_BLOCK, sigs, NULL) == -1) {
				perror("sigprocmask");
				return -1;
			}

			cp_rows = nrows;

			if (sigprocmask(SIG_UNBLOCK, sigs, NULL) == -1) {
				perror("sigprocmask");
				return -1;
			}

			if (nl == cp_rows) {
				n = linesnum(tty, cp_rows);
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

static size_t linesnum(FILE *tty, int rows)
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
			return rows;
		} else if (c == '\n') {
			return 1;
		}
	}
	return 0;
}

static int set_term_settings()
{
	if (tty_mode(0) == -1)
		return -1;

	struct termios attr;
	if (tcgetattr(tty_fileno, &attr) == -1) {
		return -1;
	}

	attr.c_lflag &= ~ECHO;
	attr.c_lflag &= ~ICANON;

	if (tcsetattr(tty_fileno, TCSANOW, &attr) == -1) {
		return -1;
	}
}

static int tty_mode(int mode)
{
	static struct termios orig_mode;
	static int orig_flags;

	if (mode == 0) {
		if (tcgetattr(tty_fileno, &orig_mode) == -1) {
			perror("tcgetattr");
			return -1;
		}

		if ((orig_flags = fcntl(tty_fileno, F_GETFL)) == -1) {
			perror("fcntl");
			return -1;
		}
	} else {
		if (tcsetattr(tty_fileno, TCSANOW, &orig_mode) == -1) {
			perror("tcsetattr");
			return -1;
		}

		if (fcntl(tty_fileno, F_SETFL, orig_flags) == -1) {
			perror("fcntl");
			return -1;
		}
	}

	return 0;
}

static int set_signal_handler(void)
{
	int sigs[] = {SIGINT, SIGWINCH};
	const int siglen = sizeof(sigs)/sizeof(int);

	struct sigaction act;
	act.sa_handler = handle_sig;

	for (int i = 0; i < siglen; i++) {
		if (sigaction(sigs[i], &act, NULL) == -1) {
			perror("sigaction");
			return -1;
		}
	}

	act.sa_flags = SA_RESTART;
	if (sigaction(SIGCONT, &act, NULL) == -1) {
		perror("sigaction");
		return -1;
	}

	act.sa_flags |= SA_RESETHAND;
	if (sigaction(SIGTSTP, &act, NULL) == -1) {
		perror("sigaction");
		return -1;
	}

	return 0;
}

// tcsh and bash will reset a terminal settings to default if a process will be
// interrupted by a signal, but let's not rely on this. Need to reset the
// default terminal settings when SIGINT and SIGTSTP are received.
static void handle_sig(int signum)
{
	switch (signum) {
		case SIGINT:
			if (tty_mode(1) == -1)
				exit(EXIT_FAILURE);
			exit(EXIT_SUCCESS);
		case SIGTSTP:
			if (tty_mode(1) == -1)
				exit(EXIT_FAILURE);
			if (raise(signum) != 0)
				exit(EXIT_FAILURE);
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
	if (ioctl(tty_fileno, TIOCGWINSZ, &wbuf) != -1) {
		nrows = wbuf.ws_row;
		ncols = wbuf.ws_col;
	}
}
