#include <stdio.h>
#include <math.h>
#include <time.h>

void bench(void (*) (), char *, int);
void inc();

int main() {
	bench(inc, "inc operations", 1e6);
}

void bench(void (*fn) (), char *op_desc, int op_count) {
	int measurements [op_count];

	int i;
	clock_t before;
	double elapsed;

	for (i = 0; i < sizeof(measurements)/sizeof(int); i++) {
		before = clock();
		(*fn)();
		elapsed = clock() - before;
		measurements[i] = (int) ((elapsed / CLOCKS_PER_SEC) * 1e9);
	}

	double sum = 0;
	for (i = 0; i < sizeof(measurements)/sizeof(int); i++) {
		sum += measurements[i];
	}
	printf("%s takes %.3f nanoseconds\n", op_desc, (double) sum/op_count);
}

void inc() { 
	int i1 = 0; 
	i1++; 
}