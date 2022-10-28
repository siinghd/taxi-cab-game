#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "ipc_utilities.h"
#include "utilities.h"

int getSemaphoreId(int key, int len)
{
    int semId = semget(key, len, IPC_CREAT | 0600);
    if (semId < 0) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    return semId;
}
int getMsgId(int key)
{
    int msqid = msgget(key, IPC_CREAT | 0600);
    if (msqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    return msqid;
}
int getSharedMemoryId(struct condivisa* sharedStruct, int key)
{
    /* Create a shared memory area */
    int shmId = shmget(key, sizeof(*sharedStruct), IPC_CREAT | 0600);
    if (shmId < 0) {
        perror("shared memory");
        exit(EXIT_FAILURE);
    }
    return shmId;
}
/* Try to access the resource */
int sem_lock(int sem_id, int sem_num)
{
    struct sembuf sops;
    sops.sem_num = sem_num;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    return semop(sem_id, &sops, 1);
}

/* Release the resource */
int sem_unlock(int sem_id, int sem_num)
{
    struct sembuf sops;
    sops.sem_num = sem_num;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    return semop(sem_id, &sops, 1);
}
/* Set a semaphore to a user defined value */
int sem_set_val(int sem_id, int sem_num, int sem_val)
{
    return semctl(sem_id, sem_num, SETVAL, sem_val);
}

int sem_operation(int sem_id, int sem_num, int op_value)
{
    struct sembuf sops;
    sops.sem_num = sem_num;
    sops.sem_op = op_value;
    sops.sem_flg = 0;
    return semop(sem_id, &sops, 1);
}
int removedIpcStruct(int shmId, int semId_Cell, int semId_Sync, int msqid)
{
    shmctl(shmId, IPC_RMID, NULL);
    printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RED "  Removed shared memory\n");
    semctl(semId_Cell, 0, IPC_RMID);
    printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RED "  Removed Semphore Cell\n");
    semctl(semId_Sync, 0, IPC_RMID);
    printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RED "  Removed Semphore Sync\n");
    while (msgctl(msqid, IPC_RMID, NULL)) {
        printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RED "  Something went wrong, can't delete MSG QUEUE\n");
    }
    printf(ANSI_COLOR_MAGENTA "MAIN" ARROWRIGHT ANSI_COLOR_RED "  Removed QUEUE\n");
}


int sem_get_val(int sem_id, int sem_num)
{
    return semctl(sem_id, sem_num, GETVAL);
}