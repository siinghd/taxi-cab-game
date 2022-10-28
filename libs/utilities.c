#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "utilities.h"
#include "initialConfig.h"
#include <math.h>
int getRandomNumberInRange(int min, int max)
{
    time_t t;
    srand((unsigned)time(&t));
    int num = (rand() % (max - min + 1)) + min;
    return num;
}

long getRandomTimeInRangeNano(long min, long max)
{
    long time;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((time_t)ts.tv_nsec);
    time = (rand() % (max - min + 1)) + min;
    return time;
}
void generateRowAndCol(int* randrow, int* randcol, int rows, int cols)
{
    long randNum = 0;
    randNum = getRandomTimeInRangeNano(0, rows * cols);
    *randcol = randNum % cols;
    *randrow = floor(randNum / cols);
}
void getHoleInRoad(int rows, int cols, struct strada matrix[rows][cols], int numHoles)
{
    int randrow = 0;
    int randcol = 0;

    int i = 0;
    int rowscounter = 0, colscounter = 0;
    while (i != numHoles) {
        do {
            generateRowAndCol(&randrow, &randcol, rows, cols);
        } while (matrix[randrow][randcol].tipoCella == 1 || matrix[randrow][randcol].tipoCella == 10);

        rowscounter = 0, colscounter = 0;
        for (rowscounter = randrow - 1; rowscounter <= randrow + 1; rowscounter++) {
            for (colscounter = randcol - 1; colscounter <= randcol + 1 && (rowscounter >= 0 && rowscounter <= rows); colscounter++) {
                if (colscounter >= 0 && colscounter <= cols) {
                    if (rowscounter == randrow && colscounter == randcol) {
                        matrix[rowscounter][colscounter].tipoCella = HOLE;
                        i++;
                    }
                    else {
                        matrix[rowscounter][colscounter].tipoCella = 10;
                    }
                }
            }
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j].tipoCella == 10) {
                matrix[i][j].tipoCella = 0;
            }
        }
    }
}

/*void setTopCells(int rows, int cols, struct strada matrix[rows][cols], int topCells)
{
    int i = 0;
    int randrow = 0;
    int randcol = 0;
    while (i != topCells) {
        do {
            generateRowAndCol(&randrow, &randcol, rows, cols);
        } while (matrix[randrow][randcol].tipoCella == HOLE || matrix[randrow][randcol].topCella == 1);
        matrix[randrow][randcol].topCella = 1;
        i++;
    }
}*/

void setStations(int rows, int cols, struct strada matrix[rows][cols], int stations)
{
    int i = 0;
    int randrow = 0;
    int randcol = 0;

    while (i != stations) {
        do {
            generateRowAndCol(&randrow, &randcol, rows, cols);
        } while (matrix[randrow][randcol].tipoCella == 1 || matrix[randrow][randcol].tipoCella == SOURCE);
        matrix[randrow][randcol].tipoCella = SOURCE;
        i++;
    }
}
void setTaxis(int rows, int cols, struct strada matrix[rows][cols], int numTaxi)
{
    int i = 0;
    int randrow = 0;
    int randcol = 0;
    while (i != numTaxi) {
        do {
            generateRowAndCol(&randrow, &randcol, rows, cols);
        } while (matrix[randrow][randcol].tipoCella == 1);
        if (matrix[randrow][randcol].taxisToBeGenerated < matrix[randrow][randcol].maxCapacityOfTaxi) {
            matrix[randrow][randcol].taxisToBeGenerated++;
            i++;
        }
    }
}
void inizializzaMatriceMappa(int rows, int cols, struct strada matrix[rows][cols], int semId, 
int numHoles, int topCellsLen, int sourcesLen, int taxisLen,long so_timensec_min , long so_timensec_max,int so_cap_min,int so_cap_max)
{
    int i, j, k, counter, id = 0, counterSem = 0;
    
    //struct sembuf sops[(rows * cols) - numHoles];
    /*
        Inizializza tutte le celle a zero con tutte le proprieta a zero
    */

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            matrix[i][j].idCella = id;
            matrix[i][j].tempoDiAttraversamento = 0;
            matrix[i][j].maxCapacityOfTaxi = 0;
            matrix[i][j].tipoCella = 0;
            matrix[i][j].numeroAttraversamenti = 0;
            //matrix[j][i].topCella = 0;

            id++;
        }
    }
    /*
       Inizializza tutte le celle con proprieta
    */
    printf("Setting holes please wait...!\n");
    getHoleInRoad(rows, cols, matrix, numHoles);
    printf("Setting holes done!\n");
    //printf("Setting setTopCells please wait...!\n");
    //setTopCells(rows, cols, matrix, SO_TOP_CELLS);
    //printf("Setting setTopCells done!\n");
    printf("Setting setStations please wait...!\n");
    setStations(rows, cols, matrix, sourcesLen);
    printf("Setting setStations done!\n");

    for (k = 0; k < rows; k++)
    {
        for (counter = 0; counter < cols; counter++)
        {
            if (matrix[k][counter].tipoCella != 1)
            {

                matrix[k][counter].tempoDiAttraversamento = getRandomTimeInRangeNano(so_timensec_min, so_timensec_max);

                matrix[k][counter].maxCapacityOfTaxi = getRandomTimeInRangeNano(so_cap_min, so_cap_max);
               
                /*if (counterSem <= (rows * cols) - numHoles)
                {
                    semctl(semId, matrix[counter][k].idCella, SETVAL, 0);
                    sops[counterSem].sem_num = matrix[counter][k].idCella;
                    sops[counterSem].sem_op = matrix[counter][k].maxCapacityOfTaxi;
                    counterSem++;
                }*/
                semctl(semId,  matrix[k][counter].idCella, SETVAL, matrix[k][counter].maxCapacityOfTaxi);
                
                //printf("%i", sem_get_val(semId, matrix[k][counter].idCella));
            }
            else
            {
                semctl(semId,  matrix[k][counter].idCella, SETVAL, 0);
            }
        }
    }
    //semop(semId, sops, counterSem);
    printf("Setting Taxis please wait....!\n");
    setTaxis(rows, cols, matrix, taxisLen);
    printf("Setting Taxis done!\n");
}

void printMap(int rows, int cols, struct strada matrix[rows][cols])
{
    for (int i = 0; i < rows; i++) {
        printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RESET "  ");
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j].tipoCella == HOLE) {
                printf(ANSI_BACK_RED "|X|" ANSI_COLOR_RESET);
            }
            else {
                if (matrix[i][j].tipoCella == SOURCE) {
                    printf(ANSI_BACK_WHITE ANSI_COLOR_BHBLACK "|S|" ANSI_COLOR_RESET);
                }
                else {
                    printf("|" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET "|", matrix[i][j].taxisToBeGenerated);
                }
            }
        }
        printf("\n");
    }
}
void test_Error(){
    if (errno) {
        printf(ANSI_COLOR_MAGENTA"ERROR!"ARROWRIGHT"  " ANSI_BACK_RED ANSI_COLOR_BHWHITE" |PID=%d| -> Error %d (%s)"ANSI_COLOR_RESET "\n",getpid(),errno,strerror(errno));
        exit(EXIT_FAILURE);
    }
}

