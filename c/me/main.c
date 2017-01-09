#include <stdio.h>
#include <stdlib.h>

// prints input file
void me(FILE *fin);

int main(void) {
    int i;
    FILE *fin;

    if ((fin = fopen(__FILE__, "r")) == NULL) {
        printf("can't open %s\n", __FILE__);
    } else {
        me(fin);
        fclose(fin);
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

        // buf doesn't end with `\0` symbol, that's why we don't know the len.
        printf("%.*s\n", i, buf);
    } while (c != EOF);
}
