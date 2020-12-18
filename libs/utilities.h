#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "initialConfig.h"
#ifndef MYLIB_H
#define MYLIB_H

#define HOLE 1
#define SOURCE 2
#define SO_WIDTH 20
#define SO_HEIGHT 10
 
 struct strada {
     int tipoCella;
     int numeroAttraversamenti;
     long tempoDiAttraversamento;
     int maxCapacityOfTaxi;
     int topCella;
     int idCella;
};
struct condivisa {
    int numOfHoles;
    int aborted;
    int success ;
    int inevasi;
    int numRichieste;
    pid_t proccNumRichieste ; 
    pid_t longhestRoute ; 
    pid_t longhestRouteCell ; 
    int longDurataTempo;
    int longRouteCell;
    pid_t so_sources_kids [SO_SOURCES]; 
    pid_t so_taxis[SO_TAXI]; 

    struct strada  cityMap [SO_WIDTH][SO_HEIGHT];
};
/* message structure */
struct message {
    long mtype;
    int xSource;
    int ySource;
    int xDestination;
    int yDestination;
};
struct timespec now;

int getRandomNumberInRange(int min, int max);
long getRandomTimeInRangeNano (long min, long max);
void getHoleInRoad(int rows,int cols,struct strada matrix[rows][cols],int numHoles);
void setTopCells(int rows,int cols,struct strada matrix[rows][cols],int topCells);
void setStations(int rows,int cols,struct strada matrix[rows][cols],int stations);

void inizializzaMatriceMappa(int rows,int cols,struct strada matrix[rows][cols],int numHoles,int semId);

#endif /* #MYLIB_H */