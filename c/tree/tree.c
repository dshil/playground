#include <stdio.h>
#include <stdlib.h>

typedef struct node node;
struct node {
	int val;
	node *left;
	node *right;
};

node *new_node(int val) {
	node *np;
	np = malloc(sizeof(node));
	np->val = val;
	np->right = NULL;
	np->left = NULL;
	return np;
}

node *insert(node *n, int val) {
	if (n == NULL) {
		return new_node(val);
	}

	if (val > n->val) {
		n->right = insert(n->right, val);
	} else if (val < n->val) {
		n->left = insert(n->left, val);
	}
	return n;
}

void print_tree(node *n) {
	if (n == NULL) { return; }
	print_tree(n->left);
	printf("el = %d\n", n->val);
	print_tree(n->right);
}

void free_tree(node *n) {
	if (n == NULL) { return; }
	free_tree(n->left);
	free_tree(n->right);
	free(n);
}

int main() {
	node *root = NULL;
	root = insert(root, 10);
	root = insert(root, 3);
	root = insert(root, 12);
	print_tree(root);
}
