#include "../include/server.h"

const char *PROJECT_NAME = "Simple TCP Server";
void welcome() { printf("Welcome to my \"%s\"!\n", PROJECT_NAME); }
void goodbye() { printf("Thank you for using my \"%s\"!\n", PROJECT_NAME); }

const int PORT = 8080;

acceptedSocket *socketList[MAX_CLIENTS] = {0};
int countSocketList = 0;
pthread_mutex_t socketListMutex = PTHREAD_MUTEX_INITIALIZER;

int main(void)
{
	//* Create socket
	int server_fd = setUpServerIPv4(PORT, MAX_CLIENTS);

#ifdef _PROD
	printf("\e[32mSUCCESS\e[0m: Socket created Successfully.\n"
				 "\e[32mSUCCESS\e[0m: Starting Server.....\n");
#endif

	//* Start of logic of the program
	while (1)
	{
		printf("\n\e[33mWAIT\e[0m: Waiting for a connection...\n");

		//* Accept a connection
		acceptedSocket *new_Client = accept_Connection(server_fd);
		if (!new_Client)
		{
			fprintf(stderr, "\e[31mERROR\e[0m: Failed to accept connection.\n");
			continue;
		}

		pthread_mutex_lock(&socketListMutex);
		if (countSocketList < MAX_CLIENTS)
			socketList[countSocketList++] = new_Client;
		pthread_mutex_unlock(&socketListMutex);

		pthread_t id;
		if (pthread_create(&id, NULL, printRecievedData, new_Client) != 0)
		{
			fprintf(stderr, "\e[31mERROR\e[0m: Failed to create thread for client.\n");
			close(new_Client->socket_fd);
			free(new_Client);
			continue;
		}
		pthread_detach(id);

#ifdef _DEBUG
		printf("\e[33mCONNECTION\e[0m: Creating a new thread with ID: %zu for the new connection with IP: %s:%d!\n",
					 (unsigned long)id,
					 inet_ntoa(new_Client->socketAddr.sin_addr),
					 ntohs(new_Client->socketAddr.sin_port));
#endif
	}
	close(server_fd);
	return 0;
	{
		//* Send data
		// const char *response =
		// "HTTP/1.1 200 OK\r\n"
		// "Content-Type: text/plain\r\n"
		// "Content-Length: 18\r\n"
		// "\r\n"
		// "Hello from server!";
		// send(new_Client->socket_fd, response, strlen(response), 0);
	}
}
