#include "network.h"

const char *PROJECT_NAME = "Simple TCP Socket Client";
void welcome() { printf("Welcome to my \"\e[96m%s\e[0m\"!\n", PROJECT_NAME); }
void goodbye() { printf("Thank you for using my \"\e[96m%s\e[0m\"!\n", PROJECT_NAME); }

const char *IP = "127.0.0.1";
const int PORT = 8080;

// #define _DEBUG
#define _PROD

int serverSetup(const char *IP, const int PORT);
void createNewThreadForRecieving(int socket_fd);
void *printMsgFromServer(void *arg);

int main(void)
{
  welcome();
  //* Create a socket and file descriptor
  int client_fd = serverSetup(IP, PORT);
#ifdef _PROD
  printf("\e[32mSUCCESS\e[0m: Connection and Socket Created!\n");
#endif
  //* Since server setup Succesfull, we do our stuff od sending and recieving data
  // char buffer[2048] = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";	//* HTTP header

  printf("What is your name: ");
  char name[51] = {0};
  fgets(name, sizeof(name), stdin);
  name[strcspn(name, "\n")] = '\0';
  //* Create a thread for listening to any recieve function
  createNewThreadForRecieving(client_fd);

  //* Start App
  printf("Enter the message (type: 'EXIT' to exit the application):\n");
  while (true)
  {
    char *inputBuffer = NULL;
    size_t inputBufferSize = 0;
    printf("-> ");
    ssize_t inputLength = getline(&inputBuffer, &inputBufferSize, stdin);
    if (inputLength < 0)
    {
      fprintf(stderr, "\e[31mERROR\e[0m: Could not read from buffer!\n");
      free(inputBuffer);
      inputBuffer = NULL;
    }
    else
    {
      inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
      inputLength = strlen(inputBuffer);
      // printf("Input buffer: [%s], Allocated Buffersize: (%zu), Input Lenght: (%ld)\n", inputBuffer, inputBufferSize, inputLenght); //* Debugging
    }
    if ((strcmp("EXIT", inputBuffer)) == 0)
    {
      close(client_fd); //* this will unblock recv()
      break;
    }
    char buffer[1024] = {0};
    size_t finalStrSize = (strlen(name) + 1 + inputLength) + 1; //* +1 for ':' and another +1 for '\0'
    snprintf(buffer, finalStrSize, "%s:%s", name, inputBuffer);
    int sendSize = send(client_fd, buffer, finalStrSize, 0);
    if (sendSize < 0)
    {
      if (errno == EPIPE || errno == ECONNRESET)
      {
        fprintf(stderr, "\e[31mERROR\e[0m: Server closed connection.\n");
        break;
      }
      perror("\e[31mWARNING\e[0m GET Request send failed");
    }
    else if (sendSize != finalStrSize)
    {
      fprintf(stderr, "\e[31mWARNING\e[0m: only %d of %zu bytes sent\n", sendSize, finalStrSize);
    }
    printf("\e[32mSUCCESS\e[0m: Sent Request to Server!\n");
    free(inputBuffer);
    inputBuffer = NULL;
  }
  close(client_fd);
  goodbye();
  return 0;
}

void *printMsgFromServer(void *arg)
{
  int socket_fd = *(int *)arg;
  free(arg); // free allocated memory

  char buffer[8192] = {0};
  int bytes_received = 0;

  while ((bytes_received = recv(socket_fd, buffer, sizeof(buffer) - 1, 0)) > 0)
  {
    buffer[bytes_received] = '\0';
    buffer[strcspn(buffer, "\n")] = '\0';
    printf("Message received: \n\e[36m-----START-----\e[0m\n%s\n\e[36m------END------\e[0m\n", buffer);
  }
  close(socket_fd);
  return NULL;
}
void createNewThreadForRecieving(int socket_fd)
{
  pthread_t id;
  int *fd_ptr = malloc(sizeof(int));
  *fd_ptr = socket_fd;
  if (pthread_create(&id, NULL, printMsgFromServer, fd_ptr) != 0)
  {
    fprintf(stderr, "\e[31mERROR\e[0m: Failed to create thread for receiving.\n");
    free(fd_ptr);
    return;
  }
  pthread_detach(id);
}
/**
 * Function to setup the socket to connect to client
 * @returns
 *  Server File Descriptor, automatically exits when connection unsuccessfull
 * @author
 *  Swarnadeep Paul
 * @param
 *  1. IP: Takes IP address as string\n
 *  2. PORT: Takes port Number
 */
int serverSetup(const char *IP, const int PORT)
{
  int client_fd;
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("\e[31mWARNING\e[0m Socket Error");
    exit(-1);
  }
#ifdef _DEBUG
  printf("\e[32mSUCCESS\e[0m: Created a socket with file descriptor: '%d'!\n", client_fd);
#endif
  //* Create server address struct and parse the neccesary
  sockaddr_in_t servAddress;
  memset(&servAddress, 0, sizeof(servAddress)); //* Empty the whole memory field
  servAddress.sin_family = AF_INET;
  servAddress.sin_port = htons(PORT);
  int status = inet_pton(AF_INET, IP, &servAddress.sin_addr);
  if (status == 0)
  {
    fprintf(stderr, "\e[31mERROR\e[0m: Invalid IP address format: %s\n", IP);
    exit(EXIT_FAILURE);
  }
  else if (status < 0)
  {
    perror("\e[31mERROR\e[0m Invalid Address / Address Not Supported");
    exit(EXIT_FAILURE);
  }
#ifdef _DEBUG
  printf("\e[32mSUCCESS\e[0m: Parsed IP: \"%s\" into byte representation: '%d'!\n", IP, servAddress.sin_addr.s_addr);
  printf("\e[32mSUCCESS\e[0m: Parsed PORT: \"%d\" into byte representation: '%d'!\n", PORT, servAddress.sin_port);
#endif
  //* Connect and Check for connection Success
  int connectStatus = 0;
  if ((connectStatus = connect(client_fd, (struct sockaddr *)&servAddress, sizeof(servAddress))) < 0)
  {
    perror("\e[31mWARNING\e[0m Connection Failed");
    exit(-1);
  }
#ifdef _DEBUG
  printf("\e[32mSUCCESS\e[0m: Connected with IP with PORT as: \"%s:%d\" with connection status: '%d'!\n", IP, PORT, connectStatus);
#endif
  return client_fd;
}
