struct read_config {
	char **argv;
	int ac; /* number of a command line arguments */
	int is_print; /* print files names or not */
	int (*read_file) (FILE *f, char *filename);
};

int read_files(struct read_config *config);
int parse_num(char *val, int *num);
