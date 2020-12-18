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
#define TYPE_DESTINATION 118
/*
 *Handler di SIGALRM or other signals;
 **/
void handle_signal(int signal);

int main(int argc, char
	const *argv[])
{
	struct message msgToSend;
	struct condivisa *sharedStruct = (struct condivisa *) malloc(sizeof(struct condivisa));
	struct sigaction sa;
	signal(SIGALRM, handle_signal);
    signal(SIGILL, handle_signal);

	int msgId = 0, shmId = 0;
	msgToSend.xSource = atoi(argv[1]);
	msgToSend.ySource = atoi(argv[2]);
	msgToSend.xDestination = 0;
	msgToSend.yDestination = 0;
	msgId = getMsgId();
	shmId = getSharedMemoryId();
	/*Create a shared memory area */
	shmId = getSharedMemoryId(sharedStruct);
	/*Attach the shared memory to a pointer */
	sharedStruct = shmat(shmId, NULL, 0);
	msgToSend.mtype = TYPE_DESTINATION;
	int msg_length = sizeof(msgToSend.xSource) *4;
	int randrow = 0;
	int randcol = 0, randNum;
	alarm(getRandomNumberInRange(1, 5));
	while (1)
	{
		do {
			randNum = getRandomTimeInRangeNano(0, SO_WIDTH *SO_HEIGHT);
			randcol = randNum % SO_HEIGHT;
			randrow = floor(randNum / SO_HEIGHT);
		} while (sharedStruct->cityMap[randrow][randcol].tipoCella == HOLE || (msgToSend.xSource == randrow && msgToSend.ySource == randcol));
		msgToSend.xDestination = randrow;
		msgToSend.yDestination = randcol;
		/*now sending the message */
		msgsnd(msgId, &msgToSend, msg_length, 0);
		printf(ANSI_COLOR_YELLOW "(PID=%d): Q_id=%d: type=%ld - xSource= %d -  ySource= %d - xDestination= %d -  yDestination= %d SENT\n",
			getpid(), msgId, msgToSend.mtype, msgToSend.xSource, msgToSend.ySource, msgToSend.xDestination, msgToSend.yDestination);

		pause();
	}

	return 0;
}

void handle_signal(int signal)
{
	if (signal == SIGALRM)
	{
		printf(ANSI_COLOR_YELLOW "SIGALARM received  waking up and sending message...\n");
		alarm(getRandomNumberInRange(1, 5));
	}else if(signal == SIGILL){
        printf(ANSI_COLOR_BLUE "SIGILL received  waking up and sending message...\n");
		alarm(getRandomNumberInRange(1, 5));
    }
}