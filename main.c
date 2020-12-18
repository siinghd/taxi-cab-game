
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

/*
 *Handler di SIGALRM or other signals;
 * 
 * 
 **/
void handle_signal(int signal);
int timeout = 0;

int main(int argc, char
	const *argv[])
{
	struct condivisa *sharedStruct = (struct condivisa *) malloc(sizeof(struct condivisa));
	struct sigaction sa;
	int so_holes = 0, top_cells = 0, so_sources = 0,
		so_cap_min = 0, so_cap_max = 0, so_taxi = 0, so_timeout = 0, so_duration = 0;
	long so_timensex_min = 0, so_timesec_max = 0;
	int semId = 0;
	int shmId = 0;
	int i = 0, j = 0;
	char *arg[4];
	signal(SIGALRM, handle_signal);
	int so_sources_kids_counter = 0;
	int child_pid = 0;
	int status = 0;
	//printf("Inserisci : SO_HOLES, SO_TOP_CELLS, SO_SOURCES, SO_CAP_MIN, SO_CAP_MAX, SO_TAXI, SO_TIMEOUT, SO_DURATION \n");
	semId = getSemaphoreId();
	/*create message queue */
	int msqid = getMsgId();
	/*Create a shared memory area */
	shmId = getSharedMemoryId(sharedStruct);
	/*Attach the shared memory to a pointer */
	sharedStruct = shmat(shmId, NULL, 0);
    sharedStruct->numOfHoles=SO_HOLES;
	inizializzaMatriceMappa(SO_WIDTH, SO_HEIGHT, sharedStruct->cityMap, SO_HOLES, semId);

	for (i = 0; i < SO_WIDTH; i++)
	{
		for (j = 0; j < SO_HEIGHT; j++)
		{
			char x[3];
			char y[3];
			sprintf(x, "%d", i);
			sprintf(y, "%d", j);
			if (sharedStruct->cityMap[i][j].tipoCella == 2)
			{
				if ((child_pid = fork()) == 0)
				{
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

	alarm(1);
	while (1)
	{
		pause();
	}

	return 0;
}

void handle_signal(int signal)
{
	struct condivisa *sharedStruct = (struct condivisa *) malloc(sizeof(struct condivisa));

	int shmId = 0;
	int child_pid = 0;
	int status = 0;
	int i = 0;
    int k ;
    int counter, counterSem=0;
	/*Create a shared memory area */
	shmId = getSharedMemoryId(sharedStruct);
	/*Attach the shared memory to a pointer */
	sharedStruct = shmat(shmId, NULL, 0);
	int msqid = getMsgId();
    int semId = getSemaphoreId();
	if (timeout == 20)
	{
		/*
		 Delete section 
		*/

		for (i = 0; i < SO_SOURCES; i++)
		{
			kill(sharedStruct->so_sources_kids[i], SIGINT);
			printf(ANSI_COLOR_RED"Killing SO_SOUCERS WITH PID %ld\n", (long) sharedStruct->so_sources_kids[i]);
		}





		/*
		 ************************************************************************************************
		 */
		for (int i = 0; i < SO_WIDTH; i++)
		{
			for (int j = 0; j < SO_HEIGHT; j++)
			{
				printf(ANSI_COLOR_RESET"|"ANSI_COLOR_GREEN"%d"ANSI_COLOR_RESET"|", sharedStruct->cityMap[i][j].tipoCella);

			}

			printf("\n");

		}
        shmctl(shmId, IPC_RMID, NULL);
        printf(ANSI_COLOR_RED"Removed shared memory\n");
        semctl(semId, 0 /* ignored */, IPC_RMID);
        printf(ANSI_COLOR_RED"Removed Semphore \n");
		while (msgctl(msqid, IPC_RMID, NULL))
		{
			printf(ANSI_COLOR_RED"Something went wrong, can't delete MSG QUEUE\n");

		}

		exit(0);
	}
	else
	{
		timeout++;
		printf(ANSI_COLOR_RESET"timeout : %d\n", timeout);
		printf(ANSI_COLOR_RESET"Richieste abortite : %d\n", sharedStruct->aborted);
		printf(ANSI_COLOR_RESET"Richieste eseguite con successo : %d\n", sharedStruct->success);
		printf(ANSI_COLOR_RESET"Richieste eseguite inevasi : %d\n", sharedStruct->inevasi);
		printf(ANSI_COLOR_RESET"Processo con piu numero di richieste : |%ld| - %d \n", (long) sharedStruct->proccNumRichieste, sharedStruct->numRichieste);
		printf(ANSI_COLOR_RESET"Processo con percorso piu lungo (celle) : |%ld| - %d \n", (long) sharedStruct->longhestRouteCell, sharedStruct->longRouteCell);
		printf(ANSI_COLOR_RESET"Processo con percorso piu lungo (tempo) : |%ld| - %d \n", (long) sharedStruct->longhestRoute, sharedStruct->longDurataTempo);
		alarm(1);

	}
}   