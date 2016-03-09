#pragma once

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct node {
	/* the three columns from the input file */
	unsigned long size; // this size corresponds to the size of the area
	char * color;
	char * name;
	/* link chain */
	struct node * next;  
	struct node * prev; 
	/* insertion order */
	struct node * insertnext; 
	int num;
	/* circle attributes */
	double x;
	double y;
	double radius;
} node_t;

static void printSVG(node_t * first, node_t * a_, node_t * bb_topright, node_t * bb_bottomleft, int debug);
static node_t * alloc_node(unsigned long size_, int num_);
static void bound(node_t * n, node_t * topright, node_t * bottomleft);
static double distance(node_t * a);
static void place(node_t * a, node_t * b, node_t * c);
static int intersects(node_t * a, node_t * b);
static void insert(node_t * a, node_t * b);
static void splice(node_t * a, node_t * b);
static node_t * placeCircles(node_t * firstnode, node_t * bb_topright, node_t * bb_bottomleft, int debug);


