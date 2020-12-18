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

#include "ipc_utilities.h"
#include "utilities.h"



int getSemaphoreId(){
    int semId = semget(SEM_KEY,(SO_WIDTH*SO_HEIGHT)-SO_HOLES, IPC_CREAT | 0600);
    if(semId<0){
               perror("semget");
               exit(EXIT_FAILURE);
    }
    return semId;
}
int getMsgId(){
    int msqid = msgget(MESSAGE_QUEUE_ID, IPC_CREAT | 0600);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }    
    return msqid;
}
int getSharedMemoryId(struct condivisa *sharedStruct){
    /* Create a shared memory area */
	int shmId = shmget(SHM_KEY, sizeof(*sharedStruct), IPC_CREAT | 0600);
        if(shmId<0){
               perror("shared memory");
               exit(EXIT_FAILURE);
    }
    return shmId;
}