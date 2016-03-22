/*
 *  packCircles 1.0
 *  
 *  author: Peter Menzel
 *
 *  ABOUT
 *  ********
 *	packCircles arranges a list of circles, which are denoted by their radii,
 *	by consecutively placing each circle externally tangent to two previously placed
 *	circles avoiding overlaps.
 *	The output is an SVG file.
 *
 *	The program implements the algorithm described in the paper:
 *  "Visualization of large hierarchical data by circle packing" 
 *  by Weixin Wang, Hui Wang, Guozhong Dai, and Hongan Wang
 *  in Proceedings of the SIGCHI Conference on Human Factors in Computing Systems, 2006, pp. 517-520
 *  https://dl.acm.org/citation.cfm?id=1124851
 
 *  Source code is partially based on a implementation of this algorithm
 *  in the ProtoVis javascript library:
 *  http://mbostock.github.io/protovis/
 *
 *  USAGE 
 *  ********
 *  Compile with: gcc -O3 -o packCircles packCircles.c -lm
 *
 *  Run with: ./packCircles -i INPUTFILE > output.svg
 *
 *  The input file has at least one unsigned long number per line that denotes the 
 *  area of the circle. 
 *  Two additional tab-separated columns can be used for the fill color and the
 *  name of each circle, which will be used in the title element for each
 *  circle.
 *  Colors need to be valid CSS color names, e.g. red, #12AB56, or rgb(123,89,12)
 *
 * */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <float.h> 
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "packCircles.h"


void usage(char *progname) { 
	fprintf(stderr, "Usage:\n  %s -i FILENAME\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Mandatory arguments:\n");
	fprintf(stderr, "   -i FILENAME   Name of input file\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Optional arguments:\n");
	fprintf(stderr, "   -c            Generate colors programmatically if not defined in input file.\n");
	fprintf(stderr, "   -d            Enable debug output.\n");
	exit(EXIT_FAILURE);
}

// h,s,v must be floats in the intervall [0,1[
static void hsv2rgb(double h, double s, double v, uint * r, uint * b, uint * g) {

    if(s==0.0) {
			*r = (uint)floor(v*256); *g = (uint)floor(v*256); *b = (uint)floor(v*256);
			return;
    }

    int i = (int)floor(h * 6);
    double f = h * 6 - i;
    double p = v * ( 1 - s );
    double q = v * ( 1 - s * f );
    double t = v * ( 1 - s * (1 - f));

    if(i == 0) {
			*r = (uint)floor(v*256); *g = (uint)floor(t*256); *b = (uint)floor(p*256);
    }
		else if(i == 1) {
			*r = (uint)floor(q*256); *g = (uint)floor(v*256); *b = (uint)floor(p*256);
    }
		else if(i == 2) {
			*r = (uint)floor(p*256); *g = (uint)floor(v*256); *b = (uint)floor(t*256);
    }
		else if(i == 3) {
			*r = (uint)floor(p*256); *g = (uint)floor(q*256); *b = (uint)floor(v*256);
    }
		else if(i == 4) {
			*r = (uint)floor(t*256); *g = (uint)floor(p*256); *b = (uint)floor(v*256);
    }
    else {
			*r = (uint)floor(v*256); *g = (uint)floor(p*256); *b = (uint)floor(q*256);
    }
}

static void printSVG(node_t * first, node_t * a_, node_t * bb_topright, node_t * bb_bottomleft, int debug) {
	double spacing = MAX(bb_topright->y + fabs(bb_bottomleft->y), bb_topright->x + fabs(bb_bottomleft->x)) / 400.0;
	double height = (bb_topright->y + fabs(bb_bottomleft->y)) + 2 * spacing;
	double width = (bb_topright->x + fabs(bb_bottomleft->x)) + 2 * spacing;
	int viewport_width = 640;
	int viewport_height = 480;
	// scaling of stroke-width with the size of the image
	double stroke_width = viewport_width / 400 * (width/viewport_width);

	printf("<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"%i\" width=\"%i\" viewBox=\"0 0 %.5f %.5f\" preserveAspectRatio=\"xMidYMid meet\">\n",viewport_height,viewport_width,width,height);
	printf("<defs>\n");
	printf("<style type=\"text/css\"><![CDATA[\n");
	printf("  .circle_c { fill:#eee; stroke: #444; stroke-width: %.5f }\n",stroke_width);
	// optionally:  printf("  .circle_c:hover { stroke: #444; stroke-width: %.5f }\n",2*stroke_width);
	printf("]]></style>\n");
	printf("</defs>\n");
	printf("<g transform=\"translate(%.5f,%.5f)\">\n",(width)/2.0,(height)/2.0);

	double offset_x = (bb_bottomleft->x + bb_topright->x) / 2.0;
	double offset_y = (bb_bottomleft->y + bb_topright->y) / 2.0;

	node_t * n = first; 
	while(n) {
		n->x -= offset_x;
		n->y -= offset_y;
		printf("<g><title>%s (num=%i)</title><circle cx=\"%.5f\" cy=\"%.5f\" r=\"%.5f\" style=\"fill:%s\" class=\"circle_c\"/></g>\n",n->name ? n->name : "" ,  (int)n->size,n->x, n->y, n->radius,n->color ? n->color : "");

		//for debug, node number
		//printf("<text x=\"%.5f\" y=\"%.5f\" stroke=\"black\" stroke-width=\"1\">%i</text>\n",n->x, n->y, n->num);
		node_t * next = n->insertnext;
		//for debug, lines follow insertion order
		//if(next) printf("<line x1=\"%.5f\" y1=\"%.5f\" x2=\"%.5f\" y2=\"%.5f\" style=\"stroke:black;stroke-width:2;\" />",n->x,n->y,next->x,next->y);
		n = next;
	}
	/* print last node chain */
	if(debug && a_ && a_->next) {
		node_t * a = a_;
		node_t * b = a_->next;
		do {
			printf("<line x1=\"%.5f\" y1=\"%.5f\" x2=\"%.5f\" y2=\"%.5f\" style=\"stroke:black;stroke-width:%.1f;\" />\n",a->x,a->y,b->x,b->y,stroke_width);
			a = b;
			b = b->next;
		} while(b != a_->next); 
	}
	printf("</g>\n");
	printf("</svg>\n");
}

static node_t * alloc_node(unsigned long size_, int num_){
  node_t * n = (node_t *)malloc(sizeof(node_t));
  n->size = size_; // this corresponds to the circle area
  //calculate radius from circle area 
  // A = pi * r^2   ->  r = sqrt(A/pi)
  double r = sqrt((double)size_ / M_PI);
  n->radius = r;
  n->next = NULL;
  n->prev = NULL;
  n->insertnext = NULL;
  n->color = NULL;
  n->name = NULL;
	n->x = 0.0;
	n->y = 0.0;
	n->num = num_;
  return n;
}

static void bound(node_t * n, node_t * topright, node_t * bottomleft) {
	bottomleft->x = MIN(n->x - n->radius, bottomleft->x);
	bottomleft->y = MIN(n->y - n->radius, bottomleft->y);
	topright->x   = MAX(n->x + n->radius, topright->x);
	topright->y   = MAX(n->y + n->radius, topright->y);
}

static double distance(node_t * a) {
	return sqrt(a->x * a->x + a->y * a->y);
}


static void place(node_t * a, node_t * b, node_t * c) {
  double da = b->radius + c->radius;
  double db = a->radius + c->radius;
  double dx = b->x - a->x;
  double dy = b->y - a->y;
  double dc = sqrt(dx * dx + dy * dy);
  double cos = (db * db + dc * dc - da * da) / (2 * db * dc);
  double theta = acos(cos);
  double x = cos * db;
  double h = sin(theta) * db;
  dx /= dc;
  dy /= dc;
  c->x = a->x + x * dx + h * dy;
  c->y = a->y + x * dy - h * dx;
}

static int intersects(node_t * a, node_t * b) {
	double dx = b->x - a->x;
	double dy = b->y - a->y;
  double dr = (double)a->radius + (double)b->radius;
 	if((dr * dr - dx * dx - dy * dy) > 0.001) // overlap is bigger than epsilon
 		return 1;
	else
		return 0; 
}


/* inserts node b after a */
static void insert(node_t * a, node_t * b) {
	node_t * c = a->next;
	a->next = b;
	b->prev = a;
	b->next = c;
	if(c) c->prev = b;
}

static void splice(node_t * a, node_t * b) {
	a->next = b;
	b->prev = a;
}

static node_t * placeCircles(node_t * firstnode, node_t * bb_topright, node_t * bb_bottomleft, int debug) {

  /* Create first circle. */
  node_t * a = firstnode;
	node_t * b = NULL;
	node_t * c = NULL;

	a->x = -1 * a->radius;
	bound(a,bb_topright,bb_bottomleft);

	/* Create second circle. */
	if(!a->insertnext) { return a; }
	b = a->insertnext;
	b->x = b->radius;
	b->y = 0;
	bound(b,bb_topright,bb_bottomleft);
	
	/* Create third circle. */
	if(!b->insertnext) { return a; }
	c = b->insertnext;
	place(a,b, c);
	bound(c,bb_topright,bb_bottomleft);

	if(!c->insertnext) { return a; }

	// make initial chain of a <-> b <-> c
	a->next = c;
	a->prev = b;
	b->next = a;
	b->prev = c;
	c->next = b;
	c->prev = a;
	b = c;
	
	//fprintf(stderr,"a=%i\n",a->num);
	//fprintf(stderr,"b=%i\n",b->num);
	//fprintf(stderr,"c=%i\n",c->num);
 	
	/* add remaining nodes */
 	int skip = 0;
	c = c->insertnext;
	while(c) {
		if(debug) fprintf(stderr,"Inserting node %i ------------------------\n",c->num);
	
		// Determine the node a in the chain, which is nearest to the center
		// The new node c will be placed next to a (unless overlap occurs)
		// NB: This search is only done the first time for each new node, i.e.
		// not again after splicing
		if(!skip) {
			node_t * n = a;
			node_t * nearestnode = n;
			double nearestdist = FLT_MAX;
			do {
				double dist_n = distance(n);
				if(dist_n < nearestdist) {	
					nearestdist = dist_n;
					nearestnode = n;
				}
				n = n->next;
			} while(n !=a );

			if(debug) fprintf(stderr,"Node %i is nearest to the origin\n",nearestnode->num);
			a = nearestnode; 
			b = nearestnode->next;
			skip=0;
		}

		if(debug) fprintf(stderr,"Trying to place node %i between nodes %i and %i\n",c->num,a->num,b->num);

		/* a corresponds to C_m, and b corresponds to C_n in the paper */
		place(a,b,c);
		
		/* for debugging: initial placement of c that may ovelap */
		//printf("<circle cx=\"%.5f\" cy=\"%.5f\" r=\"%.5f\" stroke=\"black\" stroke-width=\"3\" fill=\"red\" />\n",c->x, c->y, c->radius);

 		/* Search for possible closest intersection. */
    int isect = 0;
		int s1 = 0;
		int s2 = 0;
		node_t * j;
		for(j = b->next; j != b ; j = j->next, s1++) {
			//for debugging fprintf(stderr,"forw: testing intersection of nodes %i and %i\n",c->num,j->num);
			if(intersects(j, c)) {
				if(debug) fprintf(stderr,"Node %i intersects with node %i\n",c->num,j->num);
				isect = 1;
				break;
			}
		}
		if(isect == 1) {
			node_t * k;
			for(k = a->prev; k != j->prev; k = k->prev, s2++) {
				//for debugging fprintf(stderr,"back: testing intersection of nodes %i and %i\n",c->num,j->num);
				if(intersects(k, c)) {
					if(debug) fprintf(stderr,"Node %i intersects with node %i\n",c->num,k->num);
					if(s2 < s1) {
						isect = -1;
						j = k;
					}
					break;
				}
			}
		}
 		/* Update node chain. */
		if(isect == 0) {
			insert(a, c);
			b = c;
			bound(c,bb_topright,bb_bottomleft);
			skip = 0;
			c = c->insertnext;
		}
		else if(isect > 0) {
			if(debug) fprintf(stderr,"Forward splicing nodes a=%i and j=%i\n",a->num,j->num);
		  splice(a,j);
	    b = j;
	    skip=1;
	  } else if (isect < 0) {
			if(debug) fprintf(stderr,"Back splicing nodes j=%i and b=%i\n",j->num,b->num);
	    splice(j,b);
	    a = j;
	    skip=1;
	  }
	}

	return a;

}



int main (int argc, char **argv) {

  char *inputfilename = NULL;
	int debug = 0;
	int generate_colors = 0;

	node_t * firstnode = NULL;
	node_t * lastinsertednode = NULL;

  node_t * bb_bottomleft = alloc_node(0,-1);
	bb_bottomleft->y = (double)INT_MAX;
	bb_bottomleft->x = (double)INT_MAX;
  node_t * bb_topright = alloc_node(0,-1);
	bb_topright->y = (double)INT_MIN;
	bb_topright->x = (double)INT_MIN;

	/* parse command line */
  opterr = 0;
	int optc;
  while((optc = getopt (argc, argv, "cdi:")) != -1) {
    switch(optc) {
      case 'i':
        inputfilename = optarg;
        break;
			case 'd':
				debug = 1;
				break;
			case 'c':
				generate_colors = 1;
				break;
      default:
        usage(argv[0]);
		}
	}
	
	if(!inputfilename) {
		printf("Missing input filename.\n");
		usage(argv[0]);
	}

	FILE * fp = fopen(inputfilename,"r");
	if(!fp) {
		fprintf(stderr,"Could not open file %s\n",inputfilename);
		usage(argv[0]);
	}
	
	int num_circles = 0;
	int counter = 0;
	srand((uint)time(NULL));
	double h = generate_colors ? (double)rand()/(double)(RAND_MAX) : 0.0;
	char *line = NULL;
	size_t size = 0;;
	ssize_t length_line;
	while((length_line = getline(&line, &size, fp)) != -1) {
		if(length_line==1) { continue; } /* only newline char */
		unsigned long input_number = ULONG_MAX;
		input_number = strtoul(line,NULL,10);
		if(input_number == 0 || input_number == ULONG_MAX) { fprintf(stderr,"Bad number (out of range error) in input line: %s",line); exit(EXIT_FAILURE); }

		node_t * n = alloc_node(input_number,counter);
		if(!firstnode) {
			firstnode = n;
			lastinsertednode = n;
		} else {
			lastinsertednode->insertnext = n;
			lastinsertednode = n;
		}
		counter++;
	
		/* check for additional columns with color and name */
		char * firsttab = strchr(line,'\t');
		if(firsttab)  { 
			char * secondtab = strchr(firsttab+1,'\t');
			unsigned long length_color = 0;
			if(secondtab)  { 
				length_color = (long unsigned int)(secondtab - firsttab); //this pans out to the length of the chars between the tabs + 1 for \0 at the end
				unsigned long length_name = (long unsigned int)(line + length_line - secondtab - 1); // -1 at the end to exclude the newline
				if(length_name > 0) {
					char * name = (char *)malloc(sizeof(char) * length_name);
					strncpy(name, secondtab+1, length_name - 1 );
					name[length_name-1] = '\0';
					n->name = name;
					if(debug) fprintf(stderr,"len_name=%lu, name=%s\n",length_name,name);
				}
			}
			else { /* color is from firsttab until eol */
				length_color = (long unsigned int)(line + length_line - firsttab - 1);
			}
			if(length_color > 0) {
				char * color = (char *)malloc(sizeof(char) * length_color);
				strncpy(color, firsttab+1, length_color - 1 );
				color[length_color-1] = '\0';
				n->color = color;
				if(debug) fprintf(stderr,"len_color = %lu, color=%s\n",length_color,color);
			}
		}
		if(generate_colors && n->color==NULL) {
			uint r,g,b;
			hsv2rgb(h,0.5,0.95,&r,&g,&b);
			h += 0.618033988749895; // golden ration conjugate
			h = fmod(h,1);
			char * color = (char *)malloc(sizeof(char) * 17);
			color[16] = '\0';
			sprintf(color, "rgb(%.3u,%.3u,%.3u)",r,g,b);
			n->color = color;
		}
	}
	free(line);
	fclose(fp);
	num_circles = counter;

	if(debug) fprintf(stderr,"%i nodes are read in.\n",num_circles);
	if(!firstnode) { exit(1); }

	node_t * a = placeCircles(firstnode,bb_topright,bb_bottomleft,debug);

	printSVG(firstnode,a,bb_topright,bb_bottomleft,debug);
	
	/* go through all nodes and free */
	node_t * n = firstnode; 
	while(n) {
		node_t * next = n->insertnext;
		free(n->name);
		free(n->color);
		free(n);
		n = next;
	}
	free(bb_bottomleft);
	free(bb_topright);

	exit(EXIT_SUCCESS);
}


