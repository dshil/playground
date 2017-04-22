#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>

int main(int ac, char *av[])
{
	struct stat chld_sb, par_sb;
	DIR *dir = NULL;
	struct dirent *dp = NULL;
	char path[PATH_MAX];

	for (;;) {
		if (stat(".", &chld_sb) == -1) {
			perror("stat");
			exit(EXIT_FAILURE);
		}

		if (stat("..", &par_sb) == -1) {
			perror("stat");
			exit(EXIT_FAILURE);
		}

		if (chld_sb.st_ino == par_sb.st_ino) { /* reach the root */
			printf("\n");
			break;
		}

		if (chdir("..") == -1) {
			perror("chdir");
			exit(EXIT_FAILURE);
		}

		if ((dir = opendir(".")) == NULL) {
			perror("opendir");
			exit(EXIT_FAILURE);
		}

		for (;;) {
			errno = 0;
			dp = readdir(dir);
			if (dp == NULL) {
				if (!errno) {
					if (closedir(dir) == -1) {
						perror("closedir");
						exit(EXIT_FAILURE);
					}
					break;
				}

				perror("readdir");
				goto exit;
			}

			if (dp->d_ino == chld_sb.st_ino)
				printf("%s/", dp->d_name);
		}
	}

	exit(EXIT_SUCCESS);

exit:
	if (dir != NULL)
		if (closedir(dir) == -1)
			perror("closedir");
	exit(EXIT_FAILURE);
}
