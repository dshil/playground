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
#include <linux/limits.h>

/*
	TODO:
		1. Multi-colon output.
		2. Colorful output (to distinguish dir and file).
		3. Sort option.
		4. Author option.
		4. Directory option.
		6. Inode option.
		7. Dereference option.
		8. Recursive option.
*/

struct flags {
	char *dirname;
	int lead_dot;
	int long_format;

	int dot; /* print or not the hidden files */
	int eod; /* the last entry in the directory - we can print */
	char *dir; /* parent dir */
	char sort; /* the sorting type */
	int format; /* true - use long format, else - short format */
	size_t nidx; /* current number of file names */
	size_t nsize; /* max number of file names */
	char **names; /* files names */
};

static void
dirwalk(char *dirname, struct flags *f, void (*fcn)(char *, struct flags *));

static void fstraverse(char *dir, struct flags *f);
static void init_flag(struct flags *f); /* default flag initialization */
static void new_flag(struct flags *src, struct flags *dst);

static void finfo(char *buf, char *name, struct stat *sb);
static char* mode_to_str(mode_t m);
static char *gid_to_name(gid_t gid);
static char *uid_to_name(uid_t uid);

static int stringcmp(const void *p1, const void *p2);
static int rev_stringcmp(const void *p1, const void *p2);

int main(int ac, char *av[])
{
	struct flags f;
	init_flag(&f);

	int opt = 0;
	while((opt = getopt(ac, av, "alUr")) != -1) {
		switch(opt) {
			case 'a': f.lead_dot = 1; break;
			case 'l': f.long_format = 1; break;
			case 'U': f.sort = 'd'; break;
			case 'r': f.sort = 'r'; break;
			default:
				  exit(EXIT_FAILURE);
		}
	}

	printf("debug, optind=%d, ac=%d\n", optind, ac);

	if (ac == 1) {
		fstraverse(".", &f);
	} else {
		char **files = av + optind;
		while (--ac >= optind)
			fstraverse(*files++, &f);
	}

	free(f.names);
	exit(EXIT_SUCCESS);
}

static void init_flag(struct flags *f)
{
	f->names = (char **) malloc(1 * sizeof(char *));
	if (f->names == NULL) { /* handle the malloc error */ }

	f->nidx = 0;
	f->nsize = 1;
	f->sort = 'n';
	f->format = 0;
	f->dot = 0;
	f->eod = 0;
	f->dir = NULL;
}

static void new_flag(struct flags *src, struct flags *dst)
{
	init_flag(dst);
	dst->sort = src->sort;
	dst->format = src->format;
	dst->dot = src->dot;
}

static void
dirwalk(char *dirname, struct flags *f, void (*fcn)(char *, struct flags *))
{
	errno = 0;
	DIR *dir = opendir(dirname);
	if (dir == NULL) {
		perror("opendir");
		goto exit;
	}

	struct dirent *dp = NULL;
	for(;;) {
		errno = 0;
		dp = readdir(dir);
		if (dp == NULL) {
			if (errno == 0) {
				f->eod = 1;
				(*fcn)(NULL, f);
				break;
			}

			perror("readdir");
			goto exit;
		}

		if (strcmp(dp->d_name, ".") == 0
			|| strcmp(dp->d_name, "..") == 0)
			continue;

		(*fcn)(dp->d_name, f);
	}
	goto exit;

exit:
	if (dir != NULL) {
		if (closedir(dir) == -1)
			perror("closedir");
	}
	return;
}

static void fstraverse(char *dir, struct flags *f)
{
	if (f->eod) {
		if (f->sort == 'n')
			qsort(f->names, f->nidx, sizeof(char *), stringcmp);
		if (f->sort == 'r')
			qsort(f->names, f->nidx, sizeof(char *), rev_stringcmp);

		int i = 0;
		char *name = NULL;
		while (f->nidx-- > 0) {
			name = f->names[i++];
			printf("%s\n", name);
			free(name);
		}

		f->eod = 0;
		free(f->names);
		return;
	}

	struct stat sb;

	if (stat(dir, &sb) == -1) {
		perror(dir);
		return;
	}

	if (S_ISDIR(sb.st_mode)) {
		struct flags nf;
		new_flag(f, &nf);
		nf.dir = dir;
		dirwalk(dir, &nf, fstraverse);
		return;
	}

	if (f->nidx >= f->nsize) { /* grow the array of file names */
		size_t nz = f->nsize*2;
		char **nnames = (char **) realloc(f->names, nz * sizeof(char *));
		if (nnames == NULL) { /* handle the realloc error */ }
		f->names = nnames;
		f->nsize = nz;
	}

	if (dir[0] == '.')
		if  (!f->dot)
			return;

	char *name = NULL;
	if (f->format) {
		name = (char *) malloc(BUFSIZ * sizeof(char *));
		if (name == NULL) { /* handle the malloc error */ }
		finfo(name, dir, &sb);
	} else {
		name = (char *) malloc((strlen(dir) + 1) * sizeof(char *));
		if (name == NULL) { /* handle the malloc error */ }
		strcpy(name, dir);
	}
	f->names[f->nidx++] = name;
}

static void finfo(char *buf, char *name, struct stat *sb)
{
	sprintf(buf, "%s %4d %-8s %-8s %8ld %.12s %s",
			mode_to_str(sb->st_mode),
			sb->st_nlink,
			uid_to_name(sb->st_uid),
			gid_to_name(sb->st_gid),
			sb->st_size,
			4+ctime(&sb->st_mtime),
			name);
}

static char *mode_to_str(mode_t m)
{
	static char buf[11];
	strcpy(buf, "----------");

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

	return buf;
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

 //		if (parse_mode(mode_buf, sb.st_mode) == -1)
 //			goto error;

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
	const char *s1 = *((char **) p1);
	const char *s2 = *((char **) p2);
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
