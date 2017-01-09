#include <stdio.h>
#include <stdlib.h>

// prints input file
void me(FILE *fin);

int main(int argc, char *argv[]) {
    int i;
    FILE *fin;

    if (argc == 1) {
        // read from stdin
    } else {
        // read from file
        for (i = 1; i < argc; i++) {
            if ((fin = fopen(argv[i], "r")) == NULL) {
                printf("can't open %s\n", argv[i]);
            } else {
                me(fin);
                fclose(fin);
            }
        }
    }

    return 0;
}

void me(FILE *fin) {
    int i;
    int c;

    char buf[BUFSIZ];

    do {
        for (i = 0; (c = getc(fin)) != EOF; ) {
            buf[i++] = c;
        }

        // buf doesn't end with `/0` symbol, that's why we don't know the len.
        printf("%.*s\n", i, buf);
    } while (c != EOF);
}
