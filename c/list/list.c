#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node node;
struct node {
	int val;
	node *next;
};

node *new_node(int val) {
	node *np;
	np = (node *) malloc(sizeof(node));
	np->val = val;
	np->next = NULL;
	return np;
}


node *add_front(node *lp, node *np) {
	np->next = lp;
	return np;
}

node *add_end(node *lp, node *np) {
	if (lp == NULL) {
		return np;	
	}
	
	node *p;
	for (p = lp; p->next != NULL; p = p->next)
		;
	
	p->next = np;
	return lp;
}

node *lookup(node *lp, int val) {
	for (; lp != NULL; lp = lp->next) {
		if (lp->val == val) {
			return lp;	
		}
	}
	return NULL;
}

void print_list(node *lp) {
	for (; lp != NULL; lp = lp->next) {
		printf("val = %d\n", lp->val);
	}
}

void map(node *lp, void (*fn) (node *, void *), void *arg) {
	for (; lp != NULL; lp = lp->next) {
		(*fn)(lp, arg);
	}
}

int main() {
	node *hp;
	hp = new_node(10);
	hp = add_front(hp, new_node(200));
	hp = add_front(hp, new_node(222));
	hp = add_end(hp, new_node(1));
	print_list(hp);
	
	node *find;
	find = lookup(hp, 222);
	find == NULL ? printf("not found\n"): printf("found\n");
}
