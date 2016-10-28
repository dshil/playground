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
	printf("searching...%d\n", val);
	for (; lp != NULL; lp = lp->next) {
		if (lp->val == val) {
			return lp;	
		}
	}
	return NULL;
}

node *delete(node *lp, int val) {
	if (lp == NULL) return NULL;
	node *p, *prev;
		
	prev = NULL;
	for (p = lp; p != NULL; p = p->next) {
		if (p->val == val) {
			if (prev == NULL) {
				lp = p->next;
			} else {
				prev->next = p->next;	
			}
			printf("delete node with val = %d\n", p->val);
			free(p);
			return lp;
		}
		prev = p;
	}
	return NULL;
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
	printf("---------ADD ELEMENT-----------\n");
	hp = new_node(10);
	hp = add_front(hp, new_node(200));
	hp = add_front(hp, new_node(222));
	hp = add_end(hp, new_node(1));
	map(hp, printn, "val = %d\n");
	
	printf("---------EXPONENTION ELEMENT-----------\n");	
	map(hp, power2, NULL);
	map(hp, printn, "val = %d\n");
	
	printf("---------EVEN/ODD ELEMENT-----------\n");	
	map(hp, is_even, NULL);
	
	printf("---------LOOKUP ELEMENT-----------\n");	
	node *find;
	find = lookup(hp, 222);
	find == NULL ? printf("not found\n"): printf("found\n");
	
	printf("---------DELETE ELEMENT-----------\n");	
	hp = delete(hp, 1);
	hp = delete(hp, 100);
	hp = delete(hp, 49284);
	hp = delete(hp, 40000);
	map(hp, printn, "val = %d\n");
}
