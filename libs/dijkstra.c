#include <stdlib.h>
#include "dijkstra.h"

void path_print(path p)
{
    if (p == NULL)
    {
        printf("Empty path\n");
        return;
    }
    printf("[%i,%i]", p->my_coor.x, p->my_coor.y);
    for (; p->next != NULL; p = p->next)
    {
        printf(" -> [%i,%i]", p->next->my_coor.x, p->next->my_coor.y);
    }
    printf("\n");
}

path path_insert_node(path p, coor val)
{
    path new_el;

    /* Allocate the new node */
    new_el = malloc(sizeof(*new_el));
    /* Setting the data */
    new_el->my_coor = val;
    //printf("inserisco %d,%d", new_el->my_coor.x, new_el->my_coor.y);
    /* head insertion: old head becomes .next field of new head */
    new_el->next = p;
    /* new head is the pointer to the new node */
    return new_el;
}

path dijkstra(struct strada map[SO_HEIGHT][SO_WIDTH], coor start, coor stop)
{
    node graph[N_NODI];
    int n = 0;
    path my_path = NULL;

    for (int r = 0; r < SO_HEIGHT; r++)
    {
        for (int c = 0; c < SO_WIDTH; c++)
        {
            if (map[r][c].tipoCella != HOLE)
            {
                graph[n].my_coor.x = r;
                graph[n].my_coor.y = c;
                graph[n].prec = &graph[n];

                if (r == start.x && c == start.y)
                {
                    graph[n].cost = 0;
                    graph[n].to_visit = 1;
                }
                else
                {
                    graph[n].cost = 999;
                    graph[n].to_visit = 0;
                }
                //printf("%d : (%d,%d)->(%d,%d)\n", n, graph[n].my_coor.x, graph[n].my_coor.y, graph[n].prec->my_coor.x, graph[n].prec->my_coor.y);
                n++;
            }
        }
    }

    //dijkstra
    int search;
    do
    {
        search = 0;
        for (int i = 0; i < N_NODI; i++)
        {
            if (graph[i].to_visit == 1)
            {
                for (int k = 0; k < N_NODI; k++)
                {
                    if (((graph[k].my_coor.x == graph[i].my_coor.x + 1) && (graph[k].my_coor.y == graph[i].my_coor.y)) || ((graph[k].my_coor.x == graph[i].my_coor.x - 1) && (graph[k].my_coor.y == graph[i].my_coor.y)) || ((graph[k].my_coor.x == graph[i].my_coor.x) && (graph[k].my_coor.y == graph[i].my_coor.y + 1)) || ((graph[k].my_coor.x == graph[i].my_coor.x) && (graph[k].my_coor.y == graph[i].my_coor.y - 1)))
                    {
                        int cost_cal = 0;
                        if (graph[i].cost != 999)
                        {
                            cost_cal = graph[i].cost + 1;
                        }
                        else
                        {
                            cost_cal = 1;
                        }
                        if (graph[k].cost == 999 || cost_cal < graph[k].cost)
                        {
                            graph[k].cost = cost_cal;
                            graph[k].prec = &graph[i];
                            graph[k].to_visit = 1;
                            search = 1;
                        }
                    }
                    graph[i].to_visit = 0;
                }
            }
        }
    } while (search == 1);

    //printf("WAAAAAAAAAA\n");
    /*for (int n = 0; n < N_NODI; n++)
    {
        printf("%d : (%d,%d)->(%d,%d)\n", n, graph[n].my_coor.x, graph[n].my_coor.y, graph[n].prec->my_coor.x, graph[n].prec->my_coor.y);
    }*/
    //printf("WOOOOOOOOOOOOOO");

    //carico path
    int arrivato = 0, k = 0;
    my_path = path_insert_node(my_path, stop);

    for (int i = 0; i < N_NODI; i++)
    {
        if (graph[i].my_coor.x == stop.x && graph[i].my_coor.y == stop.y)
        {
            k = i;
        }
    }

    //printf("k:%d", k);
    //printf("Parto da |%d,%d|\n", graph[k].my_coor.x, graph[k].my_coor.y);
    while (arrivato == 0)
    {
        if (graph[k].prec->my_coor.x == start.x && graph[k].prec->my_coor.y == start.y)
        {
            arrivato = 1;
        }
        for (int n = 0; n < N_NODI; n++)
        {
            if (graph[n].my_coor.x == graph[k].prec->my_coor.x && graph[n].my_coor.y == graph[k].prec->my_coor.y)
            {

                k = n;
                //printf("Aggiungo |%d,%d|\n", graph[n].my_coor.x, graph[n].my_coor.y);
                my_path = path_insert_node(my_path, graph[n].my_coor);
                break;
            }
        }
    }
    //path_print(my_path);

    return my_path;
}

void path_free(path p)
{
    /* If p ==  NULL, nothing to deallocate */
    if (p == NULL)
    {
        return;
    }
    /* First deallocate (recursively) the next nodes... */
    path_free(p->next);
    /* ... then deallocate the node itself */
    free(p);
}

//convert integer to hexadecimal