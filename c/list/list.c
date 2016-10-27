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

node *delete(node *lp, int val) {
	if (lp == NULL) return NULL;
	node *p;
	for (p = lp; p->next->val != val; p = p->next) 
		;
	
	node *del = p->next;
	free(p->next);
	p->next = del->next;
	return lp;
}

void map(node *lp, void (*fn) (node *, void *), void *arg) {
	for (; lp != NULL; lp = lp->next) {
		(*fn)(lp, arg);
	}
}

void power2(node *n, void *arg) {
	n->val *= n->val;
}

void printn(node *n, void *arg) {
	printf((char *) arg, n->val);
}

void is_even(node *lp, void *arg) {
	lp->val % 2 == 0 ? 
		printf("val = %d is even\n", lp->val) : 
		printf("val = %d is odd\n", lp->val);
}

void freeall(node *lp) {
	node *next;
	for (; lp != NULL; lp = next) {
		next = lp->next;
		free(lp);
	}
}

int main() {
	node *hp;
	hp = new_node(10);
	hp = add_front(hp, new_node(200));
	hp = add_front(hp, new_node(222));
	hp = add_end(hp, new_node(1));

	map(hp, power2, NULL);
	map(hp, printn, "val = %d\n");	
	map(hp, is_even, NULL);
	
	node *find;
	find = lookup(hp, 222);
	find == NULL ? printf("not found\n"): printf("found\n");
	freeall(hp);
	//printf("after deletion\n");
	//hp = delete(hp, 1);
	//hp = delete(hp, 100);
	//map(hp, printn, "val = %d\n");
}
