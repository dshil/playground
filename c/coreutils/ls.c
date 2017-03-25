#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

static int r_dir(char *dirname, int is_lead_dot);

int main(int ac, char *av[])
{
	int opt = 0;
	int is_lead_dot = 0;
	while((opt = getopt(ac, av, "astF")) != -1) {
		switch(opt) {
			default:
			case 'a': is_lead_dot = 1; break;
				  exit(EXIT_FAILURE);
		}
	}

	if (ac == optind) {
		if (r_dir(".", is_lead_dot) == -1)
			exit(EXIT_FAILURE);
	} else if ((ac - optind) == 1) {
		if (r_dir(*++av, is_lead_dot) == -1)
			exit(EXIT_FAILURE);
	} else {
		while(--ac >= optind) {
			printf("%s\n", *++av);
			if (r_dir(*av, is_lead_dot) == -1)
				exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

static int r_dir(char *dirname, int is_lead_dot)
{
	struct dirent *d_ptr = NULL;

	errno = 0;
	DIR *dir = opendir(dirname);
	if (dir == NULL && errno != ENOTDIR) {
		perror("opendir");
		goto error;
	}

	if (errno == ENOTDIR) /* suppose that it's a file */
		goto success;

	for(;;) {
		errno = 0;
		d_ptr = readdir(dir);
		if (d_ptr == NULL && errno != 0) {
			perror("readdir");
			goto error;
		}
		if (d_ptr == NULL)
			break;

		if (d_ptr->d_name[0] == '.' && !is_lead_dot)
			continue;

		printf("%s\n", d_ptr->d_name);
	}

	goto success;

success:
	if (errno != ENOTDIR) {
		if (closedir(dir) == -1) {
			perror("closedir");
			goto error;
		}
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