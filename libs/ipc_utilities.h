#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#ifndef IPC_UTLITIES_H
#define IPC_UTLITIES_H
#define MESSAGE_QUEUE_ID 3240
#define SEM_KEY  3241
#define SHM_KEY  3242

int getSemaphoreId();
int getMsgId();
int getSharedMemoryId();


#endif /* #IPC_UTLITIES_H */