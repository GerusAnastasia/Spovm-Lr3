#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define BUFFER_SIZE 512

int main()
{
	int segmentID;
	char* segmentPtr = NULL;
	int semID;
	struct sembuf semBuffServer;
	struct sembuf semBuffClient; 

	int key = ftok("/home/Alexandr/Laba3/server.c", 's');
	semID = semget(key, 2, 0666 | IPC_CREAT);
	semctl(semID, 0, SETVAL, (int)0);
	semctl(semID, 1, SETVAL, (int)0);
	semBuffServer.sem_num = 0;
	semBuffServer.sem_op = -1;
	semBuffClient.sem_num = 1;
	semBuffClient.sem_op = 1;
	
	key = ftok("/home/Alexandr/Laba3/server.c", 'c');
	segmentID = shmget(key, BUFFER_SIZE, 0666 | IPC_CREAT);
		
	if (segmentID == -1)
	{
		semctl(semID, 0, IPC_RMID, 0);
		shmdt(segmentPtr);
		perror("shmget");
		exit(0);
	}
	
	printf("The server is waiting for connection with a client.\n");
	
	segmentPtr = shmat(segmentID, NULL, 0);
	
	if (segmentPtr == (char*)-1)
	{
		perror("shmat");
		exit(0);
	}
	
	while (semctl(semID, 0, GETVAL, 0) != 1);
	printf("Client connected\n");



	char str[BUFFER_SIZE];
	
	while (1)
	{
		if (!sprintf(segmentPtr, "%s", "server ready"))
		{
			break;
		}
				
		semop(semID, &(semBuffServer), 1);
		semop(semID, &(semBuffClient), 1);
		
		while (semctl(semID, 0, GETVAL, 0) != 1);
		fflush(stdin);
		fgets(str, BUFFER_SIZE, stdin);
		int i = strlen(str) - 1;
		if (str[i] == '\n')
		{
			str[i] = '\0';
		}
		if (!sprintf(segmentPtr, "%s", str))
		{	
			break;
		}
		semop(semID, &(semBuffServer), 1);
		semop(semID, &(semBuffClient), 1);

		if (strcmp(str, "quit") == 0)
		{
			usleep(1000);
			break;
		}	
		
		while (semctl(semID, 0, GETVAL, 0) != 1);
		if (sprintf(str, "%s", segmentPtr))
		{
			printf("%s\n", str);
		}
	}
	semctl(semID, 0, IPC_RMID, 0);
	shmdt(segmentPtr);
}
