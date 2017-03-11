struct read_config {
	char **argv;
	int ac; /* number of a command line arguments */
	int is_print; /* print files names or not */
	int (*file_read) (FILE *f, char *filename);
};

int files_read(struct read_config *config);
int parse_num(char *val, int *num);
