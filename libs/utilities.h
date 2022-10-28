#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "initialConfig.h"
#ifndef UTILITIES_H
#define UTILITIES_H

#define HOLE 1
#define SOURCE 2
#define SO_WIDTH 60
#define SO_HEIGHT 20
#define ID_READY_GO 0
#define ID_KIDS_READY 1

struct strada {
    int tipoCella;
    int numeroAttraversamenti;
    long tempoDiAttraversamento;
    int maxCapacityOfTaxi;
    //int topCella;
    int idCella;
    int taxisToBeGenerated;
};
struct condivisa {
    int numOfHoles;
    int aborted;
    int success;
    int inevasi;
    int numRichieste;
    pid_t procNumRichieste;
    pid_t proclonghestRouteTime;
    pid_t proclonghestRouteCell;
    int longhestRouteTime;
    int longhestRouteCell;
    int so_sources_kids_length;
    pid_t* so_sources_kids;
    int so_taxis_length;
    pid_t* so_taxis;

    struct strada cityMap[SO_HEIGHT][SO_WIDTH];
};
/* message structure */
struct message {
    long mtype;
    int xSource;
    int ySource;
    int xDestination;
    int yDestination;
};
/* message structure */
struct messageTaxi {
    long mtype;
    pid_t taxiPid;
    int longhest_route_byTime;
    int number_request;
    int longhest_route_byCell;
    ;
};
struct timespec ts;
typedef struct coor

{
    int x;
    int y;
} coor;

int getRandomNumberInRange(int min, int max);
long getRandomTimeInRangeNano(long min, long max);
void getHoleInRoad(int rows, int cols, struct strada matrix[rows][cols], int numHoles);
void setTopCells(int rows, int cols, struct strada matrix[rows][cols], int topCells);
void setStations(int rows, int cols, struct strada matrix[rows][cols], int stations);
void setTaxis(int rows, int cols, struct strada matrix[rows][cols], int numTaxi);
void printMap(int rows, int cols, struct strada matrix[rows][cols]);
void generateRowAndCol(int * randrow , int *randcol ,  int rows , int cols);
void test_Error();
void inizializzaMatriceMappa(int rows, int cols, struct strada matrix[rows][cols], int semId, 
int numHoles, int topCellsLen, int sourcesLen, int taxisLen,long so_timensec_min , long so_timensec_max,int so_cap_min,int so_cap_max);

#endif /* #UTILITIES_H */