#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
	TODO:
		1. Multi-colon output.
		2. Colorful output (to distinguish dir and file).
		3. Sort option.
		4. Author option.
		5. Directory option.
		6. Inode option.
		7. Dereference option.
		8. Recursive option.
*/

struct lsparam {
	char *dirname;
	int lead_dot;
	int long_format;
	char sort;
};

static int r_dir(struct lsparam *p);

static int
print_dir_info(char **names, int nmemb, char *dirname, int long_format);

static void print_short_format(char **names, int nmemb);
static int print_long_format(char **names, int nmemb, char *dirname);

static char *gid_to_name(gid_t gid);
static char *uid_to_name(uid_t uid);

static int parse_mode(char *buf, mode_t m);

static int stringcmp(const void *p1, const void *p2);
static int rev_stringcmp(const void *p1, const void *p2);

int main(int ac, char *av[])
{
	struct lsparam param;
	param.lead_dot = 0;
	param.long_format = 0;
	param.sort = 'n';
	param.dirname = NULL;

	int opt = 0;
	while((opt = getopt(ac, av, "alUr")) != -1) {
		switch(opt) {
			case 'a': param.lead_dot = 1; break;
			case 'l': param.long_format = 1; break;
			case 'U': param.sort = 'd'; break;
			case 'r': param.sort = 'r'; break;
			default:
				  exit(EXIT_FAILURE);
		}
	}

	if (ac == optind) {
		param.dirname = ".";
		if (r_dir(&param) == -1)
			exit(EXIT_FAILURE);
	} else if ((ac - optind) == 1) {
		av += optind;
		param.dirname = *av;
		if (r_dir(&param) == -1)
			exit(EXIT_FAILURE);
	} else {
		while(--ac >= optind) {
			printf("%s\n", *++av);
			param.dirname = *av;
			if (r_dir(&param) == -1)
				exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

static int r_dir(struct lsparam *p)
{
	errno = 0;
	DIR *dir = opendir(p->dirname);
	if (dir == NULL && errno != ENOTDIR) {
		perror("opendir");
		goto error;
	}

	if (errno == ENOTDIR) { /* suppose that it's a file */
		char *data[] = {p->dirname};
		if (print_dir_info(data, 1, p->dirname, p->long_format) == -1)
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

		if (d_ptr->d_name[0] == '.' && !p->lead_dot)
			continue;

		names[nname++] = d_ptr->d_name;
	}

	if (p->sort == 'n')
		qsort(names, nname, sizeof(char *), stringcmp);
	if (p->sort == 'r')
		qsort(names, nname, sizeof(char *), rev_stringcmp);

	if (print_dir_info(names, nname, p->dirname, p->long_format) == -1)
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

static int
print_dir_info(char **names, int nmemb, char *dirname, int long_format)
{
	if (long_format) {
		if (print_long_format(names, nmemb, dirname) == -1)
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

static int print_long_format(char **names, int nmemb, char *dirname)
{
	blkcnt_t bcnt = 0;
	struct stat sb;
	char mode_buf[11];

	char *fullname = NULL;
	const int dirname_len = strlen(dirname);

	while (nmemb-- > 0) {
		fullname = (char *) malloc(strlen(*names) + dirname_len + 2);
		strcpy(fullname, dirname);
		strcat(fullname, "/");
		strcat(fullname, *names);

		if (stat(fullname, &sb) == -1) {
			perror("stat");
			goto error;
		}
		bcnt += sb.st_blocks;

		if (parse_mode(mode_buf, sb.st_mode) == -1)
			goto error;

		printf("%s %4d %-8s %-8s %8ld %.12s %s\n",
				mode_buf,
				sb.st_nlink,
				uid_to_name(sb.st_uid),
				gid_to_name(sb.st_gid),
				sb.st_size,
				4+ctime(&sb.st_mtime),
				*names);

		names++;
		free(fullname);
	}
	printf("total %d\n", bcnt/2);
	return 0;

error:
	free(fullname);
	return -1;
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
	if (strcpy(buf, "----------") == NULL) {
		perror("strcpy");
		return -1;
	}

	if (S_ISDIR(m)) buf[0] = 'd';
	if (S_ISCHR(m)) buf[0] = 'c';
	if (S_ISBLK(m)) buf[0] = 'b';

	if (m & S_IRUSR) buf[1] = 'r';
	if (m & S_IWUSR) buf[2] = 'w';
	if (m & S_IXUSR) buf[3] = 'x';
	if (m & S_ISUID) buf[3] = 's';

	if (m & S_IRGRP) buf[4] = 'r';
	if (m & S_IWGRP) buf[5] = 'w';
	if (m & S_IXGRP) buf[6] = 'x';
	if (m & S_ISGID) buf[6] = 's';

	if (m & S_IROTH) buf[7] = 'r';
	if (m & S_IWOTH) buf[8] = 'w';
	if (m & S_IXOTH) buf[9] = 'x';
	if (m & S_ISVTX) buf[9] = 't';

	return 0;
}

static int stringcmp(const void *p1, const void *p2)
{
	// p1, p2 in the realiaty are `char **`
	// strcasecmp expects to get `const char *`
	// *(char * const *) = char * const = const char *
	const char *s1 = *((char * const *) p1);
	const char *s2 = *((char * const *) p2);
	return strcasecmp(s1, s2);
}

static int rev_stringcmp(const void *p1, const void *p2)
{
	char *s1 = *((char **) p1);
	char *s2 = *((char **) p2);

	char c1 = 0;
	char c2 = 0;
	for (;;) {
		c1 = tolower(*s1++);
		c2 = tolower(*s2++);

		if (c1 == c2)
			if (c1 == '\0')
				break;
			else
				continue;
		return (c1 > c2) ? -1 : 1;
	}

	return -1;
}
