#include "../include/server.h"

acceptedSocket *accept_Connection(const int server_fd)
{
	acceptedSocket *newClient = malloc(sizeof(acceptedSocket));
	if (!newClient)
		return NULL;
	newClient->client_addrLen = sizeof(newClient->socketAddr);
	newClient->socket_fd = accept(server_fd, (struct sockaddr *)&(newClient->socketAddr), (socklen_t *)&(newClient->client_addrLen));
	newClient->connctSuccess = (bool)(newClient->socket_fd >= 0);
	newClient->error = (!newClient->connctSuccess) ? newClient->socket_fd : 0;
	return newClient;
}
void *printRecievedData(void *args)
{
	acceptedSocket *client_Socket = (acceptedSocket *)args;

	// * Get the client IP and print
	printf("\e[32mINCOMING\e[0m Connection from: \e[31m%s\e[0m on the port: %d\n", inet_ntoa(client_Socket->socketAddr.sin_addr), ntohs(client_Socket->socketAddr.sin_port));

	//* Receive data
	char buffer[8192] = {0};
	int bytes_received = 0;
	printf("Client message: \n");
	while ((bytes_received = recv(client_Socket->socket_fd, buffer, (sizeof(buffer) - 1), 0)) > 0)
	{
		buffer[bytes_received] = '\0';
		buffer[strcspn(buffer, "\n")] = '\0';
		printf("\e[36m-----START-----\e[0m\n%s\n\e[36m------END------\e[0m\n", buffer);
		//* Send It to all other users
		// sendRecievedMsgToALL(buffer, client_Socket->socket_fd);
	}
	printf("\e[31mCLOSING\e[0m Connection closed with the IP: \e[31m%s\e[0m!\n\n", inet_ntoa(client_Socket->socketAddr.sin_addr));
	shutdown(client_Socket->socket_fd, SHUT_WR);
	close(client_Socket->socket_fd);
	free(client_Socket);
	return NULL;
}
void sendRecievedMsgToALL(const char *buffer, const int src_socket_fd)
{
	for (int i = 0; i < countSocketList; i++)
	{
		if ((socketList[i]->socket_fd) != src_socket_fd)
		{
			send(socketList[i]->socket_fd, buffer, strlen(buffer), 0);
		}
	}
}
