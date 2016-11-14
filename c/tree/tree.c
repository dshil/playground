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

node *look_up(node *n, int val) {
	if (n == NULL) { return NULL; }
	if (n->val > val) {
		return look_up(n->left, val);
	} else if (n->val < val) {
		return look_up(n->right, val);
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
	printf("free element %d\n", n->val);
	free(n);
}

int tree_height(node *n) {
	if (n == NULL) { return 0; }
	int l_h = tree_height(n->left);
	int r_h = tree_height(n->right);
	return r_h > l_h ? r_h + 1 : l_h + 1;
}

int main() {
	node *root = NULL;
	root = insert(root, 10);
	root = insert(root, 3);
	root = insert(root, 12);
	root = insert(root, 11);
	root = insert(root, 15);
	root = insert(root, 14);
	root = insert(root, 16);
	print_tree(root);
	printf("Height = %d\n", tree_height(root));
	node* fn = look_up(root, 3);
	printf("Find node with val = %d\n", fn->val);
	
	node *nd = look_up(root, 1000);
	if (nd == NULL) {
		printf("node with val = 1000 was not found\n");
	}	
	//free_tree(root);
	//print_tree(root);
}
