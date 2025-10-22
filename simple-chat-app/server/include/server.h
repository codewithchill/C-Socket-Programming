#ifndef _SERVER_H_
#define _SERVER_H_

// #define _DEBUG
#define _PROD

#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef struct sockaddr_in sockaddr_in_t;

typedef struct acceptedSocket {
	sockaddr_in_t socketAddr;
	socklen_t client_addrLen;
	int socket_fd;
	int error;
	bool connctSuccess;
} acceptedSocket;

extern const int PORT;
#define MAX_CLIENTS 10
extern acceptedSocket *socketList[MAX_CLIENTS];
extern int countSocketList;
extern pthread_mutex_t socketListMutex;


int setUpServerIPv4(const int _PORT, const int connection_backlog);
acceptedSocket *accept_Connection(const int server_fd);
void *printRecievedData(void *args);
void	sendRecievedMsgToALL(const char *buffer, const int src_socket_fd);

#endif /*_SERVER_H_*/