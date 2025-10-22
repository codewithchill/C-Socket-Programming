#include "../include/server.h"

int setUpServerIPv4(const int _PORT, const int connection_backlog)
{
	int server_fd = -1;
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("\e[31mWARNING\e[0m Socket Error");
		exit(-1);
	}
#ifdef _DEBUG
	printf("\e[32mSUCCESS\e[0m: Created a socket with file descriptor: '%d'!\n", server_fd);
#endif
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		perror("\e[31mWARNING\e[0m Function \"setsockopt()\" failed");
		close(server_fd);
		exit(1);
	}
#ifdef _DEBUG
	printf("\e[32mSUCCESS\e[0m: Socket Option set success!\n");
#endif
	//* Bind socket to address and port
	sockaddr_in_t address;
	size_t addrlen = sizeof(address);
	memset(&address, 0, addrlen);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(_PORT);
	if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0)
	{
		perror("\e[31mWARNING\e[0m Bind Failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
#ifdef _DEBUG
	printf("\e[32mSUCCESS\e[0m: Bind successfull!\n");
#endif
	// Listen for connections
	if (listen(server_fd, connection_backlog) != 0)
	{
		perror("\e[31mWARNING\e[0m Listening Error");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
#ifdef _DEBUG
	printf("\e[32mSUCCESS\e[0m: Listen Success with n = %d\n", connection_backlog);
#endif
	return server_fd;
}
