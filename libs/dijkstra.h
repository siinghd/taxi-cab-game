#include "utilities.h"

#include <stdlib.h>

/*#define N_ARCHI (((SO_WIDTH - 1) * (SO_HEIGHT - 1)) * 2) + (SO_WIDTH - 1) + (SO_HEIGHT - 1)
#define N_NODI (SO_WIDTH * SO_HEIGHT) - SO_HOLES
#define MAX_INT 11*/


typedef struct node
{
    coor my_coor;
    int to_visit;
    int cost;
    struct node *prec; /* pointer to the same structure */
} node;

typedef struct path_node
{
    coor my_coor;
    struct path_node *next;
} path_node;

typedef path_node *path;

void path_print(path p);
path path_insert_node(path p, coor val);
path dijkstra(struct strada map[SO_HEIGHT][SO_WIDTH], coor start, coor stop);
void path_free(path p);