#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <strings.h>
#include <math.h>
#include "debug.h"
#include <iostream>
using namespace std;
#define MAX_OBJ (1000*1000)
#define N 4 // 最大3つ入る

typedef struct _DATA {
	int key;
	int val;
	struct _DATA *next;
} DATA;

typedef struct _NODE {
	bool isLeaf;
	struct _NODE *chi[N];
	int key[N-1]; 
	int nkey;
	struct _NODE *parent;
	struct _NODE *next;
} NODE;

// typedef struct _TEMP {
// 	bool isLeaf;
// 	NODE *chi[N+1]; // for internal split (for leaf, only N is enough)
// 	int key[N]; // for leaf split
// 	int nkey;
// } TEMP;

// DATA Head;
// DATA *Tail;

// NODE *Root = NULL;

DATA *alloc_data(int key);
NODE *alloc_node();
NODE *alloc_leaf();

NODE *find_leaf(NODE *node, int key);
DATA *
find_data(NODE *node, int key, NODE **leaf_out);
NODE *
insert_in_leaf(NODE *leaf, int key, DATA *data);
NODE * 
insert_in_node_after_splitting(NODE *node, NODE *old_node, int left_index, int key, NODE *right);
NODE * 
insert_in_parent(NODE *root, NODE *left, int key, NODE *right);
NODE *
insert_in_leaf_after_splitting(NODE *root, NODE *leaf, int key, DATA *data);
NODE *insert_in_new_root(NODE *left, int key, NODE *right);
NODE *init_new_tree(int key, DATA *data);
NODE *insert(NODE *root, int key);