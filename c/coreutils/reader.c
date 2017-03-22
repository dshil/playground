#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "reader.h"

int read_files(struct read_config *conf)
{
	int i = 0;
	FILE *f = NULL;
	int is_additional_space = 0;

	for (i = optind; i < conf->ac; i++) {
		if (conf->argv[i] == NULL)
			continue;

		if ((f = fopen(conf->argv[i], "r")) == NULL) {
			perror("fopen");
			return -1;
		}

		if (conf->is_print) {
			if (is_additional_space)
				printf("\n");

			if (i != (conf->ac - 1))
				is_additional_space = 1;
			else
				is_additional_space = 0;

			printf("==> %s <==\n", conf->argv[i]);
		}

		if (conf->read_file(f) == -1) {
			if (fclose(f) == -1) {
				perror("fclose");
				return -1;
			}
		}

		if (fclose(f) == -1) {
			perror("fclose");
			return -1;
		}
	}

	return 0;
}

int parse_num(char *val, int *num)
{
	if (val == NULL)
		return 0;

	errno = 0;
	int n = atoi(val);
	if (errno != 0) {
		perror("atoi");
		return -1;
	}

	*num = n;
	return 0;
}

int read_and_print_bytes(FILE *f, size_t nmemb)
{
	char buf[nmemb];
	ssize_t n = 0;
	const int len = sizeof(buf)/sizeof(buf[0]);

	n = fread(buf, 1, len, f);
	if (ferror(f) != 0) {
		perror("fread");
		return -1;
	}

	if (fwrite(buf, 1, n, stdout) != n) {
		perror("fwrite");
		return -1;
	}

	return 0;
}
