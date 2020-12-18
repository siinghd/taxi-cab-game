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
    srand(time(NULL)); 
    int num = (rand() % (max - min + 1)) + min; 
    return num;
} 

long getRandomTimeInRangeNano (long min, long max){
    long time;
    clock_gettime(CLOCK_REALTIME ,&now);
	time = (now.tv_nsec % (max - min + 1)) + min; 
    return time;
}

void getHoleInRoad(int rows,int cols,struct strada matrix[rows][cols],int numHoles){
    int randrow = 0;
    int randcol = 0;
    long randNum=0;
    int i=0;
    int rowscounter =0, colscounter=0;
    while ( i < numHoles)
    {
        do{
            randNum= getRandomNumberInRange(0,rows*cols);
            randcol  = randNum % cols;
            randrow= floor( randNum / cols );
            
           
        }while(matrix[randrow][randcol].tipoCella==1 || matrix[randrow][randcol].tipoCella==10);
        
        rowscounter =0, colscounter=0;
        for ( rowscounter = randrow-1; rowscounter <= randrow+1;rowscounter++)
        {
            for ( colscounter = randcol-1; colscounter <= randcol+1 && (rowscounter>=0 && rowscounter<=rows)  ; colscounter++)
            {
                if(colscounter>=0 && colscounter<=cols){
                    if(rowscounter==randrow && colscounter == randcol){
                        matrix[rowscounter][colscounter].tipoCella=HOLE;
                        i++;
                    }
                    else{
                         matrix[rowscounter][colscounter].tipoCella=10;            
                    }
                }
            }   
               
        }
    }
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                if(matrix[i][j].tipoCella==10){
                    matrix[i][j].tipoCella=0;
                }
            }
        } 
}


void setTopCells(int rows,int cols,struct strada matrix[rows][cols],int topCells){
    int i = 0 ;
    int randrow = 0;
    int randcol = 0;
    long randNum =0;
    while (i<topCells)
    {
        do{
                        randNum= getRandomNumberInRange(0,rows*cols);
            randcol  = randNum % cols;
            randrow= floor( randNum / cols );
        }while(matrix[randrow][randcol].tipoCella==HOLE ||   matrix[randrow][randcol].topCella==1);
        matrix[randrow][randcol].topCella=1;
        i++;
    }
}

void setStations(int rows,int cols,struct strada matrix[rows][cols],int stations){
    int i = 0 ;
    int randrow = 0;
    int randcol = 0;
    long randNum =0;

    while (i<stations)
    {
        do{
                                    randNum= getRandomNumberInRange(0,rows*cols);
            randcol  = randNum % cols;
            randrow= floor( randNum / cols );
        }while(matrix[randrow][randcol].tipoCella==1 ||   matrix[randrow][randcol].tipoCella==SOURCE);
        matrix[randrow][randcol].tipoCella=SOURCE;
        i++;
    }
}

void inizializzaMatriceMappa(int rows,int cols,struct strada matrix[rows][cols],int numHoles,int semId){
    int i ,j ,k,counter , id =0 , counterSem=0;;
    struct sembuf sops[(rows*cols)-numHoles];
    /*
        Inizializza tutte le celle a zero con tutte le proprieta a zero
    */

    for ( i = 0; i < rows; i++)
    {
        for ( j = 0; j < cols; j++)
        {
            matrix[i][j].idCella= id;
            matrix[i][j].tempoDiAttraversamento=0;
            matrix[i][j].maxCapacityOfTaxi= 0;
            matrix[i][j].tipoCella= 0;
            matrix[i][j].numeroAttraversamenti= 0;
            matrix[i][j].topCella= 0;

            id++;
        }
    }
     /*
        Inizializza tutte le celle con proprieta
    */

    getHoleInRoad(rows,cols,matrix,numHoles);
    printf("Setting holes done!\n");
    setTopCells(rows,cols,matrix,30);
    printf("Setting setTopCells done!\n");

    setStations(rows,cols,matrix,SO_SOURCES);
    printf("Setting setStations done!\n");

    for ( k = 0; k < rows; k++)
    {
        for ( counter = 0; counter < cols; counter++)
        {
            if(matrix[k][counter].tipoCella!=1 ){
               
                matrix[k][j].tempoDiAttraversamento= getRandomTimeInRangeNano(SO_TIMENSEC_MIN,SO_TIMENSEC_MAX);


                matrix[k][counter].maxCapacityOfTaxi= getRandomNumberInRange(SO_CAP_MIN,SO_CAP_MAX);
                if( counterSem <= (rows*cols)-numHoles ){
             
                        sops[counterSem].sem_num = matrix[k][counter].idCella;

                        sops[counterSem].sem_op = matrix[k][counter].maxCapacityOfTaxi;
                        counterSem++;
     

                }
               
            }

        }
        
    }
     semop(semId, sops, counterSem);
    
}