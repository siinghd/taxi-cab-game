#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"
#ifndef IPC_UTLITIES_H
#define IPC_UTLITIES_H
#define MESSAGE_QUEUE_KEY 3240
#define SEM_CELL_KEY 3241
#define SHM_KEY 3242
#define SEM_SYNC_KEY 3243
#define SEM_LENGTH 2


#define TYPE_DESTINATION 118
#define TYPE_TAXI_MAX_INFO 119
#define TYPE_TAXI_ABORTED 120
#define TYPE_TAXI_SUCCESS 121
#define TYPE_TAXI_INEVEVASI 122
#define TYPE_DELETE 999

int getSemaphoreId(int key, int len);
int getMsgId(int key);
int getSharedMemoryId(struct condivisa* sharedStruct, int key);
int removedIpcStruct(int shmId, int semId_Cell, int semId_Sync, int msqid);
/* Try to access the resource */
int sem_lock(int sem_id, int sem_num);

/* Release the resource */
int sem_unlock(int sem_id, int sem_num);
int sem_set_val(int sem_id, int sem_num, int sem_val);
int sem_operation(int sem_id, int sem_num, int op_value);
int sem_get_val(int sem_id, int sem_num);
#endif /* #IPC_UTLITIES_H */