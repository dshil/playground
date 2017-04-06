#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

static int parse_mode(char *rec, mode_t *m);
static int setmode(char utype, char mode, mode_t *m);
static int set_user_access(char utype, char mode, mode_t *m);
static int set_group_access(char utype, char mode, mode_t *m);
static int set_other_access(char utype, char mode, mode_t *m);

int main(int ac, char *av[])
{
	if (ac == optind) {
		fprintf(stderr, "%s: missing operand\n", av[0]);
		exit(EXIT_FAILURE);
	}

	char *rec = *++av;
	char *p, *q = NULL;
	mode_t mode = 0;

	for (q = rec; (p = strtok(q, ",")) != NULL; q = NULL) {
		if (parse_mode(p, &mode) == -1)
			exit(EXIT_FAILURE);
	}

	while(--ac != optind) {
		if (chmod(*++av, mode) == -1) {
			perror("chmod");
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

static int parse_mode(char *rec, mode_t *mode)
{
	char *s_mode = rec;
	char *s_utype = rec;
	int i = 0; /* position of the ``=`` */
	while (*s_mode++ != '=') i++;

	while(*s_utype != '=') {
		while (*s_mode != '\0') {
			if (setmode(*s_utype, *s_mode, mode) == -1)
				return -1;
			s_mode++;
		}
		s_utype++;
		s_mode = rec + i + 1;
	}
	return 0;
}

static int setmode(char utype, char mode, mode_t *m)
{
	if (utype == 'u') {
		if (set_user_access(utype, mode, m) == -1) return -1;
	} else if (utype == 'g') {
		if (set_group_access(utype, mode, m) == -1) return -1;
	} else if (utype == 'o') {
		if (set_other_access(utype, mode, m) == -1) return -1;
	} else if (utype == 'a') {
		if (set_user_access(utype, mode, m) == -1 ||
		set_group_access(utype, mode, m) == -1 ||
		set_other_access(utype, mode, m) == -1)
			return -1;
	} else {
		fprintf(stderr,
				"%s: invalid owner type: `%c=%c`\n", "chmod", utype, mode);
		return -1;
	}

	return 0;
}

static int set_user_access(char utype, char mode, mode_t *m)
{
	if (mode == 'r') *m |= S_IRUSR;
	else if (mode == 'w') *m |= S_IWUSR;
	else if (mode == 'x') *m |= S_IXUSR;
	else if (mode == 's') *m |= S_ISUID;
	else if (mode == 't') *m |= S_ISVTX;
	else {
		fprintf(stderr,
				"%s: invalid mode: `%c=%c`\n", "chmod", utype, mode);
		return -1;
	}

	return 0;
}

static int set_group_access(char utype, char mode, mode_t *m)
{
	if (mode == 'r') *m |= S_IRGRP;
	else if (mode == 'w') *m |= S_IWGRP;
	else if (mode == 'x') *m |= S_IXGRP;
	else if (mode == 's') *m |= S_ISUID;
	else if (mode == 't') *m |= S_ISVTX;
	else {
		fprintf(stderr,
				"%s: invalid mode: `%c=%c`\n", "chmod", utype, mode);
		return -1;
	}

	return 0;
}

static int set_other_access(char utype, char mode, mode_t *m)
{
	if (mode == 'r') *m |= S_IROTH;
	else if (mode == 'w') *m |= S_IWOTH;
	else if (mode == 'x') *m |= S_IXOTH;
	else if (mode == 's') *m |= S_ISUID;
	else if (mode == 't') *m |= S_ISVTX;
	else {
		fprintf(stderr,
				"%s: invalid mode: `%c=%c`\n", "chmod", utype, mode);
		return -1;
	}

	return 0;
}
