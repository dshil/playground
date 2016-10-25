#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum { NVINIT = 1, NVGROW = 2 };

typedef struct vector vector;
struct vector {
	int nval;
	int max;
	int *data;
};

int len(vector *v);

void print_vector(vector *v) {
	if (v == NULL) return;
	
	int i;
	for (i = 0; i < v->nval; i++) {
		printf("%d\n", v->data[i]);
	}
}

void add(vector *v, int val) {
	if (v == NULL) return;
	
	if (v->data == NULL) {
		// first element in the vector
		// need to allocate memory for 1 element
		v->data =  malloc(NVINIT * sizeof(v->data[0]));
		if (v->data == NULL) return;
		v->max = NVINIT;
	} else if (v->nval >= v->max){
		// grow the size of the vector
		int *cp = realloc(v->data, (v->max * NVGROW) * sizeof(v->data[0]));
		if (cp == NULL) return;
		v->data = cp;

		v->max *= NVGROW;
	}
	v->data[v->nval] = val;
	v->nval++;
}

void del_without_order(vector *v, int val) {
	if (v == NULL) return;
	
	int i;
	for (i = 0; i < v->nval; i++) {
		if (v->data[i] == val) {
			v->data[i] = v->data[v->nval - 1];
			v->nval--;	
			return;
		}
	}

	fprintf(stderr, "val = %d was not found in vector\n", val);
}

void del_with_order(vector *v, int val) {
	if (v == NULL) return;
	
	int i;
	for (i = 0; i < v->nval; i++) {
		if (v->data[i] == val) {
			int move_len = (v->nval - i - 1) * sizeof(int);	
			memmove(v->data + i, v->data + i + 1, move_len);
			v->nval--;
			return;
		}
	}
}

bool is_valid_index(vector *v, int index) {
	return index >= 0 && index < v->nval;
}

int get(vector *v, int index) {
	if (v == NULL) return - 1;
	if (!is_valid_index(v, index)) {
		printf("invalid index\n");
		return -1;
	}
	return v->data[index];
}

void set(vector *v, int val, int index) {
	if (v == NULL) return;
	if (v->data == NULL) {
		printf("vector is empty\n");
		return;
	}
	if (!is_valid_index(v, index)) {
		printf("invalid index\n");
		return;
	}
	v->data[index] = val;
}

int len(vector *v) {
	if (v == NULL) return -1;
	return v->nval;
}

vector v;

int main() {
	int i;
	for (i = 0; i < 10; i++) {
		add(&v, i);	
	}
	printf("before deletion\n");
	print_vector(&v);

	del_with_order(&v, 1);
	printf("after deletion\n");
	print_vector(&v);

	printf("elem with index 3 = %d\n", get(&v, 3));
}
