#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "libs/utilities.h"
#include "libs/initialConfig.h"
#include "libs/ipc_utilities.h"

#include "libs/dijkstra.h"

/*int main(int argc, char const *argv[])
{
    struct messageTaxi msgToSend;
    struct condivisa *sharedStruct = (struct condivisa *)malloc(sizeof(struct condivisa));

    int msgId = 0, shmId = 0, semId_Sync = 0;
    msgId = getMsgId(MESSAGE_QUEUE_KEY);
    semId_Sync = getSemaphoreId(SEM_SYNC_KEY, 2);
    //Create a shared memory area
    shmId = getSharedMemoryId(sharedStruct, SHM_KEY);
    //Attach the shared memory to a pointer
    sharedStruct = shmat(shmId, NULL, 0);
    msgToSend.mtype = TYPE_TAXI_SUCCESS;
    sem_unlock(semId_Sync, ID_KIDS_READY);
    //printf(ANSI_COLOR_MAGENTA"SO_TAXI" ARROWRIGHT  ANSI_COLOR_CYAN"  PID=%d, Waiting for father!\n", getpid());
    sem_lock(semId_Sync, ID_READY_GO);

    int msg_length = sizeof(msgToSend.longhest_route_byTime) * 4 + sizeof(msgToSend.taxiPid);
    msgToSend.taxiPid = getpid();
    printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, sending MSG of type %ld\n", getpid(), msgToSend.mtype);
    msgsnd(msgId, &msgToSend, msg_length, 0);

    return 0;
    
}*/
void movement(int rows , int cols , struct strada city_map[rows][cols], int sem_cell, coor start, coor stop, coor *taxi, int *route_cells, int *route_time);
void handle_signal(int signal);

int main(int argc, char const *argv[])
{
    //printf("Printing values!\n");
    struct coor taxi_coor;
    struct message rcv_msg;
    struct condivisa *shared_struct = (struct condivisa *)malloc(sizeof(struct condivisa));
    struct sigaction sa;

    //signal(SIGUSR1, handle_signal);
    signal(SIGALRM, handle_signal);
    int semId_Sync = getSemaphoreId(SEM_SYNC_KEY, SEM_LENGTH);
    int sem_cell = getSemaphoreId(SEM_CELL_KEY, ( SO_HEIGHT * SO_WIDTH));

    int msg_id = 0, shm_id = 0;
    printf("%i,%i",  atoi(argv[2]),atoi(argv[1]));
    taxi_coor.x = atoi(argv[1]);
    taxi_coor.y = atoi(argv[2]);

    msg_id = getMsgId(MESSAGE_QUEUE_KEY);
    /*Create a shared memory area */
    shm_id = getSharedMemoryId(shared_struct, SHM_KEY);
    /*Attach the shared memory to a pointer */
    shared_struct = shmat(shm_id, NULL, 0);
    long mtype = TYPE_DESTINATION;
    int msg_length = sizeof(rcv_msg.xSource) * 4;
    int num_bytes;

    //stats
    struct messageTaxi msg_stats;
    int msg_snd_length = sizeof(msg_stats.longhest_route_byTime) * 4 + sizeof(msg_stats.taxiPid);

    msg_stats.taxiPid = getpid();
    msg_stats.longhest_route_byCell = 0;
    msg_stats.longhest_route_byTime = 0;
    int this_route_cells;
    int this_route_time;

    //blocco la cella in cui mi sono creato
    printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, entering cell (%d,%d) with capacity %i\n", getpid(), taxi_coor.y, taxi_coor.x, sem_get_val(sem_cell, shared_struct->cityMap[taxi_coor.y][taxi_coor.x].idCella));
    sem_lock(sem_cell, shared_struct->cityMap[taxi_coor.x][taxi_coor.y].idCella);
    //printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, i'm in cell (%d,%d) with capacity %i\n", getpid(), taxi_coor.y, taxi_coor.x, sem_get_val(sem_cell, shared_struct->cityMap[taxi_coor.y][taxi_coor.x].idCella));

    printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, operative on (%d,%d)\n", getpid(), taxi_coor.y, taxi_coor.x);
    sem_unlock(semId_Sync, ID_KIDS_READY);
    sem_lock(semId_Sync, ID_READY_GO);

    while (1)
    {
        //VADO SU UNA SOURCE
        //la cerco(le 8 celle attorno poi le 16 attorno alle 8 ecc...)

        coor best_source;
        int best_source_finded = 0, n = 1;

        while (best_source_finded == 0)
        {
            for (int r = taxi_coor.x - n; r <= taxi_coor.x + n + 1; r++)
            {
                for (int c = taxi_coor.y - n; c <= taxi_coor.y + n + 1 && (SO_HEIGHT > r && r >= 0); c++)
                {
                    if ((SO_WIDTH > c && c >= 0) && shared_struct->cityMap[r][c].tipoCella == SOURCE && best_source_finded == 0)
                    {
                        best_source.x = r;
                        best_source.y = c;
                        printf("%i,%i", best_source.x, best_source.y);
                        best_source_finded = 1;
                    }
                }
            }
            n++;
        }

        //mi sposto verso la source
        printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, I go in source [%i,%i]\n", getpid(), best_source.x, best_source.y);
        movement(SO_HEIGHT,SO_WIDTH,shared_struct->cityMap, sem_cell, taxi_coor, best_source, &taxi_coor, &this_route_cells, &this_route_time);
        printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, arrivato source\n", getpid());

        //ricevo il messaggio
        printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, I'm listening\n", getpid());
        int msg_find = 0;
        coor start, stop;
        while (msg_find == 0)
        {
            if(msgrcv(msg_id, &rcv_msg, msg_length, mtype, 0)!=-1){
                 printf("|%d| sono in [%d][%d]-[%d][%d] e cerco di prendere il messaggio\n",getpid(),rcv_msg.xSource,rcv_msg.ySource,best_source.x,best_source.y);
                 
                if (rcv_msg.xSource == best_source.x && rcv_msg.ySource == best_source.y) //se è un messaggio della mia source
                {
                    //prendo info dal messaggio
                    start.x = rcv_msg.xSource;
                    start.y = rcv_msg.ySource;
                    stop.x = rcv_msg.xDestination;
                    stop.y = rcv_msg.yDestination;
                    msg_find = 1;
                    msg_stats.number_request++;
                } else
                {
                    //lo rimetto nella coda
                    msgsnd(msg_id, &rcv_msg, msg_snd_length, 0);
                    //printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  (PID=%d): Q_id=%d: type=%ld |xSource= %d -  ySource= %d| " ARROWRIGHT "  |xDestination= %d -  yDestination= %d| SENT\n", getpid(), msg_id, rcv_msg.mtype, rcv_msg.xSource, rcv_msg.ySource, rcv_msg.xDestination, rcv_msg.yDestination);
                }
            }

           
        }
        printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, msg_id=%d, msg_type=%ld ,new route=(%d,%d)to(%d,%d)\n", getpid(), msg_id, rcv_msg.mtype, rcv_msg.xSource, rcv_msg.ySource, rcv_msg.xDestination, rcv_msg.yDestination);

        //mi muovo
        this_route_cells = 0;
        this_route_time = 0;
        movement(SO_HEIGHT,SO_WIDTH,shared_struct->cityMap, sem_cell, start, stop, &taxi_coor, &this_route_cells, &this_route_time);
        printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, SONO ARRIVATO\n",getpid());

        msg_stats.mtype = TYPE_TAXI_SUCCESS;
        msgsnd(msg_id, &msg_stats, msg_length, 0);

        //controllo semaforo cella
        //semaforo libero
        //mi sposto
        //semaforo bloccato
        //inizia timer attesa e mi fermo
        //se timer finisce
        //creo nuovo taxi
        //mi uccido
        //else
        //mi sposto
    }
    return 0;
}

void handle_signal(int signal)
{
    if (signal == SIGALRM)
    {
        printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, I'm exploding...\n", getpid());
        printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, Creating my successor...\n", getpid());
        //cambiare nellarray di pid dei taxi quello nuovo con quello corrente
        printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, Killing myself\n", getpid());
        //invio messaggio al main per la stampa finale(mettere ricezione nel main)
        //sem_unlock(sem_cell, city_map[taxi->x][taxi->y].idCella);
        raise(SIGINT);
    }
    else if (signal == SIGINT)
    {
    }
}

void movement(int rows , int cols ,struct strada city_map[SO_HEIGHT][SO_WIDTH], int sem_cell, coor start, coor stop, coor *taxi, int *route_cells, int *route_time)
{

    path my_path = NULL;
    my_path = dijkstra(city_map, start, stop);

    struct timespec time1, time2;
    time1.tv_sec = 0;


    printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, Calculated path = ", getpid());
    path_print(my_path);


    printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, I start in [%d,%d]\n", getpid(), my_path->my_coor.x, my_path->my_coor.y);
    for (; my_path->next != NULL; my_path = my_path->next)
    {

        printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, Entering cell (%d,%d) with capacity %i\n", getpid(), my_path->next->my_coor.x, my_path->next->my_coor.y, sem_get_val(sem_cell, city_map[my_path->next->my_coor.x][my_path->next->my_coor.y].idCella));
        alarm(SO_TIMEOUT);
        sem_lock(sem_cell, city_map[my_path->next->my_coor.x][my_path->next->my_coor.y].idCella);
        sem_unlock(sem_cell, city_map[taxi->x][taxi->y].idCella);
  
        city_map[taxi->x][taxi->y].numeroAttraversamenti++;

        taxi->x = my_path->next->my_coor.x;
        taxi->y = my_path->next->my_coor.y;
        printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, Leaving cell (%d,%d) with capacity %i\n", getpid(), taxi->x, taxi->y, sem_get_val(sem_cell, city_map[taxi->x][taxi->y].idCella));
        printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, I'm in cell (%d,%d) with capacity %i\n", getpid(), my_path->next->my_coor.x, my_path->next->my_coor.y, sem_get_val(sem_cell, city_map[my_path->next->my_coor.x][my_path->next->my_coor.y].idCella));

        time1.tv_nsec = city_map[taxi->x][taxi->y].tempoDiAttraversamento;
        printf(ANSI_COLOR_MAGENTA "SO_TAXI" ARROWRIGHT ANSI_COLOR_CYAN "  PID=%d, I'm taking %li to cross\n", getpid(), city_map[taxi->x][taxi->y].tempoDiAttraversamento);
        nanosleep(&time1, &time2);

        *route_cells++;
    }
    //*route_time = 10;
    printf("\n");
    path_free(my_path);
}
