#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

static int r_dir(char *dirname, int lead_dot, int long_format);
static int print_dir_info(char **names, int nmemb, int long_format);
static void print_short_format(char **names, int nmemb);
static int print_long_format(char **names, int nmemb);

static char *gid_to_name(gid_t gid);
static char *uid_to_name(uid_t uid);

static int parse_mode(char *buf, mode_t m);

static int stringcmp(const void *p1, const void *p2);

int main(int ac, char *av[])
{
	int opt = 0;
	int lead_dot = 0;
	int long_format = 0;
	while((opt = getopt(ac, av, "al")) != -1) {
		switch(opt) {
			case 'a': lead_dot = 1; break;
			case 'l': long_format = 1; break;
			default:
				  exit(EXIT_FAILURE);
		}
	}

	if (ac == optind) {
		if (r_dir(".", lead_dot, long_format) == -1)
			exit(EXIT_FAILURE);
	} else if ((ac - optind) == 1) {
		av += optind;
		if (r_dir(*av, lead_dot, long_format) == -1)
			exit(EXIT_FAILURE);
	} else {
		while(--ac >= optind) {
			printf("%s\n", *++av);
			if (r_dir(*av, lead_dot, long_format) == -1)
				exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

static int r_dir(char *dirname, int lead_dot, int long_format)
{
	errno = 0;
	DIR *dir = opendir(dirname);
	if (dir == NULL && errno != ENOTDIR) {
		perror("opendir");
		goto error;
	}

	if (errno == ENOTDIR) { /* suppose that it's a file */
		char *data[] = {dirname};
		if (print_dir_info(data, 1, long_format) == -1)
			goto error;

		return 0;
	}

	// TODO: optimize want
	char *names[1000];
	int nname = 0;

	struct dirent *d_ptr = NULL;

	for(;;) {
		errno = 0;
		d_ptr = readdir(dir);
		if (d_ptr == NULL) {
			if (errno == 0)
				break;

			perror("readdir");
			goto error;
		}

		if (d_ptr->d_name[0] == '.' && !lead_dot)
			continue;

		names[nname++] = d_ptr->d_name;
	}

	qsort(names, nname, sizeof(char *), stringcmp);
	if (print_dir_info(names, nname, long_format) == -1)
		goto error;
	goto success;

success:
	if (closedir(dir) == -1) {
		perror("closedir");
		goto error;
	}
	return 0;

error:
	if (dir != NULL) {
		if (closedir(dir) == -1) {
			perror("closedir");
		}
	}
	return -1;
}

static int print_dir_info(char **names, int nmemb, int long_format)
{
	if (long_format) {
		if (print_long_format(names, nmemb) == -1)
			return -1;
	} else
		print_short_format(names, nmemb);

	return 0;
}

static void print_short_format(char **names, int nmemb)
{
	while (nmemb-- > 0)
		printf("%s\n", *names++);
}

static int print_long_format(char **names, int nmemb)
{
	struct stat sb;
	char mode_buf[12];

	while (nmemb-- > 0) {
		if (stat(*names, &sb) == -1) {
			perror("stat");
			return -1;
		}

		if (parse_mode(mode_buf, sb.st_mode) == -1)
			return -1;

		printf("%s %d %-2s %-2s %5ld %.12s %s\n",
				mode_buf,
				sb.st_nlink,
				uid_to_name(sb.st_uid),
				gid_to_name(sb.st_gid),
				sb.st_size,
				4+ctime(&sb.st_mtime),
				*names);

		names++;
	}

	return 0;
}

static char *uid_to_name(uid_t uid)
{
	static char numstr[10];
	struct passwd *pp = NULL;

	if ((pp = getpwuid(uid)) == NULL) {
		sprintf(numstr, "%d", uid);
		return numstr;
	}

	return pp->pw_name;
}

static char *gid_to_name(gid_t gid)
{
	static char numstr[10];
	struct group *gp = NULL;

	if ((gp = getgrgid(gid)) == NULL) {
		sprintf(numstr, "%d", gid);
		return numstr;
	}

	return gp->gr_name;
}

static int parse_mode(char *buf, mode_t m)
{
	if (strcpy(buf, "-----------") == NULL) {
		perror("strcpy");
		return -1;
	}

	if (S_ISDIR(m)) buf[0] = 'd';
	if (S_ISCHR(m)) buf[0] = 'c';
	if (S_ISBLK(m)) buf[0] = 'b';

	if (m & S_IRUSR) buf[1] = 'r';
	if (m & S_IWUSR) buf[2] = 'w';
	if (m & S_IXUSR) buf[3] = 'x';

	if (m & S_IRGRP) buf[4] = 'r';
	if (m & S_IWGRP) buf[5] = 'w';
	if (m & S_IXGRP) buf[6] = 'x';

	if (m & S_IROTH) buf[7] = 'r';
	if (m & S_IWOTH) buf[8] = 'w';
	if (m & S_IXOTH) buf[9] = 'x';

	buf[10] = '.';

	return 0;
}

static int stringcmp(const void *p1, const void *p2)
{
	return strcmp(* (char * const *) p1, * (char * const *) p2);
}
