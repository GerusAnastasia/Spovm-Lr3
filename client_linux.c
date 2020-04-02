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
	semID = semget(key, 0, 0666);
	if (semID == -1)
	{
		exit(0);
	}
	semBuffServer.sem_num = 0;
	semBuffServer.sem_op = 1;
	semBuffClient.sem_num = 1;
	semBuffClient.sem_op = -1;
	
	key = ftok("/home/Alexandr/Laba3/server.c", 'c');
	segmentID = shmget(key, BUFFER_SIZE, 0666);
		
	if (segmentID == -1)
	{
		perror("shmget");
		exit(0);
	}
	
	segmentPtr = shmat(segmentID, NULL, 0);
	
	if (segmentPtr == (char*)-1)
	{
		perror("shmat");
		exit(0);
	}
	
	printf("Connected to server.\n");
	semop(semID, &(semBuffServer), 1);


	char str[BUFFER_SIZE];

	while (1)
	{
		while (semctl(semID, 1, GETVAL, 0) != 1);
		if (sprintf(str, "%s", segmentPtr))
		{
			printf("%s\n", str);
		}
		
		semop(semID, &(semBuffClient), 1);
		semop(semID, &(semBuffServer), 1);		

		
		while (semctl(semID, 1, GETVAL, 0) != 1);
		if (sprintf(str, "%s", segmentPtr))
		{
			printf("%s\n", str);
			if (strcmp(str, "quit") == 0)
			{
				break;
			}
			strcpy(str, "the client has received a message");
			if (!sprintf(segmentPtr, "%s", str))
			{
				break;
			}
		}
		
		semop(semID, &(semBuffClient), 1);		
		semop(semID, &(semBuffServer), 1);

	}
}
