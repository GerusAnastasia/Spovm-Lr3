#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>  
#define BUFFER_SIZE 512

int main() {
	HANDLE  hNamedPipe;
	char pipeName[] = "\\\\.\\pipe\\demo_pipe";
	hNamedPipe = CreateFile(
		pipeName, GENERIC_WRITE | GENERIC_READ,
		0, NULL, OPEN_EXISTING, 0, NULL);

	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		printf("Connection with the named pipe failed.");
		_getch();
		return 0;
	}

	HANDLE hSemServer = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("SemServer"));
	HANDLE hSemClient = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("SemClient"));

	printf("Connected to server.\n");

	ReleaseSemaphore(hSemServer, 1, NULL);

	char str[BUFFER_SIZE];
	while (strcmp(str, "exit"))
	{
		DWORD dwRead;
		if (ReadFile(hNamedPipe, str,
			BUFFER_SIZE, &dwRead, NULL))
		{
			printf("%s\n", str);
		}
		ReleaseSemaphore(hSemServer, 1, NULL);

		WaitForSingleObject(hSemClient, INFINITE);

		if (ReadFile(hNamedPipe, str,
			BUFFER_SIZE, &dwRead, NULL))
		{
			printf("%s\n", str);
			DWORD dwWritten;
			if (!WriteFile(hNamedPipe, "the client has received a message", BUFFER_SIZE,
				&dwWritten, NULL))
			{
				break;
			}
		}
		ReleaseSemaphore(hSemServer, 1, NULL);
	}

	CloseHandle(hNamedPipe);
	CloseHandle(hSemClient);
	printf("Press key to finish the client:  ");
	_getch();
	return 0;
}