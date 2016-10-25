#include <stdio.h>
#include <stdlib.h>

void swap(int a[], int i, int j);
void rec_sort(int a[], int n);
void print_array(int a[], int n);

int main() {
	int a[] = {3, 4, 1, 10, 5, 6};
	int len = sizeof(a)/sizeof(int);
	rec_sort(a, len);
	print_array(a, len);
}

void rec_sort(int a[], int n) {
	if (n <= 1)
		return;
	
	swap(a, 0, rand() % n);
	int last = 0;
	
	for (int i = 1; i < n; i++) {
		if (a[i] < a[0]) {
			swap(a, ++last, i);
		}		
	}

	swap(a, 0, last);

	rec_sort(a, last);
	rec_sort(a + last + 1, n - last - 1);
}

void swap(int a[], int i, int j) {
	int tmp = a[i];
	a[i] = a[j];
	a[j] = tmp;
}

void iter_sort(int a[], int n) {
	swap(a, 0, rand() % n);
	int last = 0;
	for (int i = 1; i < n; i++)
		if (a[i] < a[0])
			swap(a, ++last, i);
	
	swap(a, 0, last);
}

void print_array(int a[], int n) {
	for (int i = 0; i < n; i++) {
		printf("%d ", a[i]);	
	}
	printf("\n");
}
