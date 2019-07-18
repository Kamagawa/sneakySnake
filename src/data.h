#ifndef datas
#define datas


#define EDGE_R 48
#define EDGE_C 64
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define gibRando(lower, upper, seed) ((seed) % ((upper)-(lower))+(lower))

typedef struct {
	int x;
	int y;
} Tuple;

typedef struct {
	int* x;
	int* y;
} Tupleptr;

#endif