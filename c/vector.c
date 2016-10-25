#include <stdio.h>
#include <stdlib.h>

enum { NVINIT = 1, NVGROW = 3 };

typedef struct vector vector;
struct vector {
	int nval;
	int max;
	int *data;
};

void print_vector(vector *v) {
	if (v == NULL) {
		return;
	}

	for (int i = 0; i < v->nval; i++) {
		printf("%d\n", v->data[i]);
	}
}

void add(vector *v, int val) {
	if (v == NULL) return;

	if (v->data == NULL) {
		// first element in the vector
		// need to allocate memory for 1 element
		v->data =  malloc(NVINIT * sizeof(int));
		if (v->data == NULL) return;
		v->nval++;
		v->data[0] = val;
		v->max = NVINIT;
	} else if (v->nval >= v->max){
		// grow the size of the vector
		int *cp = realloc(v->data, (v->max * NVGROW) * sizeof(int));
		if (cp == NULL) return;

		v->data = cp;
		v->max *= NVGROW;
		v->data[v->nval++] = val;
	} else {
		v->data[v->nval++] = val;
	}
}

void del_without_order(vector *v, int val) {
	if (v == NULL) return;
	
	for (int i = 0; i < v->nval; i++) {
		if (v->data[i] == val) {
			v->data[i] = v->data[v->nval - 1];
			v->nval--;	
			return;
		}
	}

	fprintf(stderr, "val = %d was not found in vector\n", val);
}

vector v;

int main() {
	for (int i = 0; i < 2; i++) {
		add(&v, i);	
	}
	print_vector(&v);
	del_without_order(&v, 111);
}
