
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
#include <sys/shm.h>
#include <sys/msg.h>
#include "libs/utilities.h"
#include "libs/initialConfig.h"
#include "libs/ipc_utilities.h"
#include "libs/mylib.h"

/*
 *Handler di SIGALRM or other signals;
 * 
 * 
 **/
void handle_signal(int signal);
int so_duration = 0;

int main(int argc, char const* argv[])
{
    struct condivisa* sharedStruct = (struct condivisa*)malloc(sizeof(struct condivisa));
    struct sigaction sa;
    /************/
    int so_holes = 0, top_cells = 0, so_sources = 0,
        so_cap_min = 0, so_cap_max = 0, so_taxi = 0, so_timeout = 0;
    long so_timensec_min = 0, so_timesec_max = 0;
    /************/
    char * line = malloc(sizeof(char)*100);
    FILE *fp = fopen("config.txt", "r");
    while ((line = readLine(fp))!=NULL)
    {
        char ** splitted = split(line,' ');
        if(strcmp(splitted[0],"SO_HOLES")==0){
            so_holes= atoi(splitted[1]);
        }else if (strcmp(splitted[0],"SO_TOP_CELLS")==0)
        {
             top_cells= atoi(splitted[1]);
        }else if (strcmp(splitted[0],"SO_SOURCES")==0)
        {
             so_sources= atoi(splitted[1]);
        }else if (strcmp(splitted[0],"SO_CAP_MIN")==0)
        { 
             so_cap_min= atoi(splitted[1]);
        }else if (strcmp(splitted[0],"SO_CAP_MAX")==0)
        {
             so_cap_max= atoi(splitted[1]);
        }else if (strcmp(splitted[0],"SO_TAXI")==0)
        {
             so_taxi= atoi(splitted[1]);
        }else if (strcmp(splitted[0],"SO_TIMENSEC_MIN")==0)
        {
             so_timensec_min= atol(splitted[1]);
        }else if (strcmp(splitted[0],"SO_TIMENSEC_MAX")==0)
        {
             so_timesec_max= atol(splitted[1]);
        }
        else if (strcmp(splitted[0],"SO_TIMEOUT")==0)
        {
              so_timeout= atoi(splitted[1]);     
        }
        else if (strcmp(splitted[0],"SO_DURATION")==0)
        {
             so_duration= atoi(splitted[1]);
             
        }
        
    }
    fclose(fp);
    if (line)
        free(line);
    int semId_Cell = 0;
    int semId_Sync = 0;
    int shmId = 0;
    int i = 0, j = 0;
    char* arg[4];
    signal(SIGALRM, handle_signal);
    signal(SIGINT, handle_signal);

    int so_sources_kids_counter = 0;
    int so_taxis_kids_counter = 0;
    int child_pid = 0;
    int status = 0;
    pid_t so_sources_kids[so_sources];
    pid_t so_taxi_kids[so_taxi];
    //printf("Inserisci : SO_HOLES, SO_TOP_CELLS, SO_SOURCES, SO_CAP_MIN, SO_CAP_MAX, SO_TAXI, SO_TIMEOUT, SO_DURATION \n");
    semId_Cell = getSemaphoreId(SEM_CELL_KEY, (SO_HEIGHT * SO_WIDTH));
    semId_Sync = getSemaphoreId(SEM_SYNC_KEY, SEM_LENGTH);

    /*create message queue */
    int msqid = getMsgId(MESSAGE_QUEUE_KEY);
    /*Create a shared memory area */
    shmId = getSharedMemoryId(sharedStruct, SHM_KEY);
    /*Attach the shared memory to a pointer */
    sharedStruct = shmat(shmId, NULL, 0);
    sharedStruct->numOfHoles = so_holes;
    sharedStruct->so_sources_kids_length = so_sources;
    sharedStruct->so_sources_kids = so_sources_kids;
    sharedStruct->so_taxis_length = so_taxi;
    sharedStruct->so_taxis = so_taxi_kids;
    inizializzaMatriceMappa(SO_HEIGHT, SO_WIDTH, sharedStruct->cityMap, semId_Cell, so_holes, top_cells, so_sources, so_taxi,so_timensec_min,so_timesec_max,so_cap_min,so_cap_max);
    semctl(semId_Sync, ID_READY_GO, SETVAL, 0);
    semctl(semId_Sync, ID_KIDS_READY, SETVAL, 0);
    char x[3];
    char y[3];
    for (i = 0; i < SO_HEIGHT; i++) {
        for (j = 0; j < SO_WIDTH; j++) {

            sprintf(x, "%d", i);
            sprintf(y, "%d", j);
            if (sharedStruct->cityMap[i][j].tipoCella == 2) {
                if ((child_pid = fork()) == 0) {
                    arg[0] = "./so_sources";
                    arg[1] = x;
                    arg[2] = y;
                    arg[3] = NULL;
                    execvp(arg[0], arg);
                }

                sharedStruct->so_sources_kids[so_sources_kids_counter++] = child_pid;
            }
        }
    }
    int g_counter = 0;
    char z[3];
    char f[3];
    for (i = 0; i < SO_HEIGHT; i++) {
        for (j = 0; j < SO_WIDTH; j++) {
            for (g_counter = 0; g_counter < sharedStruct->cityMap[i][j].taxisToBeGenerated; g_counter++) {

                sprintf(z, "%d", i);
                sprintf(f, "%d", j);

                if ((child_pid = fork()) == 0) {
                    arg[0] = "./taxi";
                    arg[1] = z;
                    arg[2] = f;
                    arg[3] = NULL;
                    execvp(arg[0], arg);
                }
                sharedStruct->so_taxis[so_taxis_kids_counter++] = child_pid;
            }
        }
    }
    /* 
	 * Il padre aspetta che tutti  siano pronti prima di dargli le
	 * risorse "ingrediente":  soltanto dopo che tutti  i NUM_PROC
	 * figli avranno eseguito la loro semop che incrementa di 1 la
	 * risorsa, si potranno consumare NUM_PROC risorse
	 */
    sem_operation(semId_Sync, ID_KIDS_READY, -so_taxi);
    printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_CYAN " KIDS! ready\n");
    sem_operation(semId_Sync, ID_READY_GO, so_taxi);

    alarm(1);
    while (1) {
        pause();
    }

    return 0;
}

void handle_signal(int signal)
{
    struct condivisa* sharedStruct = (struct condivisa*)malloc(sizeof(struct condivisa));
    struct messageTaxi msgTaxi;
    struct msqid_ds my_queue_stat;
    int msgLength = sizeof(msgTaxi.longhest_route_byTime) * 4 + sizeof(msgTaxi.taxiPid);
    int shmId = 0;
    int child_pid = 0;
    int status = 0;
    int i = 0;
    int k;
    int counter, counterSem = 0;
    int num_bytes = 0;
    /*Create a shared memory area */
    shmId = getSharedMemoryId(sharedStruct, SHM_KEY);
    /*Attach the shared memory to a pointer */
    sharedStruct = shmat(shmId, NULL, 0);
    int msqid = getMsgId(MESSAGE_QUEUE_KEY);
    int semId_Cell = getSemaphoreId(SEM_CELL_KEY, (SO_HEIGHT * SO_WIDTH));
    int semId_Sync = getSemaphoreId(SEM_SYNC_KEY, SEM_LENGTH);
    if (signal == SIGALRM) {
        printf("%d",so_duration);
        if (so_duration == 0) {
            /*
				Delete section 
				*/
            printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RED "  Killing SO_SOUCERS\n");
            for (i = 0; i < sharedStruct->so_sources_kids_length; i++) {
                kill(sharedStruct->so_sources_kids[i], SIGINT);
            }
            printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RED "  Killing SO_TAXI \n");
            for (i = 0; i < sharedStruct->so_taxis_length; i++) {
                kill(sharedStruct->so_taxis[i], SIGINT);
            }

            /*
				************************************************************************************************
				*/

            printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_YELLOW "  Reading Data...\n");
            while (my_queue_stat.msg_qnum != 0) {
                /* now receiving the message */
                num_bytes = msgrcv(msqid, &msgTaxi, msgLength, 0, 0);
                if (num_bytes >= 0) {
                    if (msgTaxi.mtype == TYPE_TAXI_ABORTED) {
                        sharedStruct->aborted++;
                    }
                    else if (msgTaxi.mtype == TYPE_DESTINATION) {
                        sharedStruct->inevasi++;
                    }
                    else if (msgTaxi.mtype == TYPE_TAXI_SUCCESS) {
                        sharedStruct->success++;
                        if (msgTaxi.number_request > sharedStruct->numRichieste) {
                            sharedStruct->procNumRichieste = msgTaxi.taxiPid;
                            sharedStruct->numRichieste = msgTaxi.number_request;
                        }
                        if (msgTaxi.longhest_route_byCell > sharedStruct->longhestRouteCell) {
                            sharedStruct->proclonghestRouteCell = msgTaxi.taxiPid;
                            sharedStruct->longhestRouteCell = msgTaxi.longhest_route_byCell;
                        }
                        if (msgTaxi.longhest_route_byTime > sharedStruct->longhestRouteTime) {
                            sharedStruct->proclonghestRouteTime = msgTaxi.taxiPid;
                            sharedStruct->longhestRouteTime = msgTaxi.longhest_route_byTime;
                        }
                    }
                }
                msgctl(msqid, IPC_STAT, &my_queue_stat);
            }

            printMap(SO_HEIGHT, SO_WIDTH, sharedStruct->cityMap);
            printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RESET "  Richieste abortite : %d\n", sharedStruct->aborted);
            printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RESET "  Richieste eseguite con successo : %d\n", sharedStruct->success);
            printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RESET "  Richieste eseguite inevasi : %d\n", sharedStruct->inevasi);
            printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RESET "  Processo con piu numero di richieste : |%ld| - %d \n", (long)sharedStruct->procNumRichieste, sharedStruct->numRichieste);
            printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RESET "  Processo con percorso piu lungo (celle) : |%ld| - %d \n", (long)sharedStruct->proclonghestRouteCell, sharedStruct->longhestRouteCell);
            printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RESET "  Processo con percorso piu lungo (tempo) : |%ld| - %d \n", (long)sharedStruct->proclonghestRouteTime, sharedStruct->proclonghestRouteTime);
            removedIpcStruct(shmId, semId_Cell, semId_Sync, msqid);
            exit(0);
        }
        else {
            so_duration--;
            printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RESET "  ********************" ANSI_COLOR_RED "TIMEOUT : %d" ANSI_COLOR_RESET "********************\n", so_duration);
            printMap(SO_HEIGHT, SO_WIDTH, sharedStruct->cityMap);
            alarm(1);
        }
    }
    else if (signal == SIGINT) {
        removedIpcStruct(shmId, semId_Cell, semId_Sync, msqid);
        exit(0);
    }
}