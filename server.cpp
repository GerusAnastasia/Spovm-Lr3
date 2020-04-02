#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h> 
#define BUFFER_SIZE 512

int main() {
	HANDLE  hNamedPipe;     // создаем именованный канал 
	hNamedPipe = CreateNamedPipe("\\\\.\\pipe\\demo_pipe", PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, BUFFER_SIZE, BUFFER_SIZE, NMPWAIT_USE_DEFAULT_WAIT, NULL);
	// проверяем на успешное создание 
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		printf("error\n");
		_getch();
		return 0;
	}

	HANDLE hSemServer = CreateSemaphore(NULL, 0, 1, "SemServer");
	HANDLE hSemClient = CreateSemaphore(NULL, 0, 1, "SemClient");
	// ждем пока клиент свяжется с каналом 
	printf("the server is waiting for connection with a client.\n");

	if (!ConnectNamedPipe(
		hNamedPipe, NULL))
	{
		CloseHandle(hNamedPipe);
		printf("error with connection. press key to finish the server: ");
		_getch();
		return 0;
	}

	WaitForSingleObject(hSemServer, INFINITE);
	printf("client connected.\n");

	char str[BUFFER_SIZE];
	while (strcmp(str, "exit"))
	{
		DWORD dwWritten1;
		if (!WriteFile(hNamedPipe, "server is ready.\n", BUFFER_SIZE,
			&dwWritten1, NULL))
		{
			break;
		}
		ReleaseSemaphore(hSemClient, 1, NULL);

		WaitForSingleObject(hSemServer, INFINITE);
		fflush(stdin);
		gets_s(str);
		DWORD dwWritten2;
		if (!WriteFile(hNamedPipe, str, BUFFER_SIZE,
			&dwWritten2, NULL))
		{
			break;
		}
		ReleaseSemaphore(hSemClient, 1, NULL);

		WaitForSingleObject(hSemServer, INFINITE);

		DWORD dwRead;
		if (ReadFile(hNamedPipe, str, BUFFER_SIZE,
			&dwRead, NULL))
		{
			printf("%s\n", str);
		}

	}

	CloseHandle(hNamedPipe);
	CloseHandle(hSemServer);
	printf("Press key to finish the server: ");
	_getch();
	return 0;
}