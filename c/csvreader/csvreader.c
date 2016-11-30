#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buf[200];
char *fields[20];

int csvgetline(FILE *fin) {
	if (fgets(buf, sizeof(buf), fin) == NULL) {
		return -1;
	}

	char *p, *q;
	
	int nfield = 0;
	for (q = buf; (p = strtok(q, ",\n\r")) != NULL; q = NULL) {
		fields[nfield++] = p;
	}
	
	return nfield;
}

int main() {
	int nf;
	int i;

	while((nf = csvgetline(stdin)) != -1) {
		for (i = 0; i < nf; i++) {
			printf("field[%d] = '%s'\n",i, fields[i]);
		} 
	}
}
