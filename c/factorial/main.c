#include <stdio.h>
#include <stdlib.h>

int factorial(int n) {
	if (n < 0) {
		printf("input value must be > 0\n");
		exit(-1);
	}
	if (n <= 1) {
		return 1;
	}

	return n * factorial(n - 1);
}

int main() {
	printf("factorial 1 = %d\n", factorial(1));
	printf("factorial 0 = %d\n", factorial(0));
	printf("factorial 5 = %d\n", factorial(5));
	printf("factorial 10 = %d\n", factorial(10));
	printf("factorial -1 = %d\n", factorial(-1));
}