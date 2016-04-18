//CLIENT.CPP
#include <stdlib.h>
#include <stdio.h>      
#include <sys/types.h>    
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>

using namespace std;

#define DEFAULT_IP "192.168.1.217"
#define SERVER_PORT 5432
#define CLIENT_PORT 5433
#define MAX_INPUT 256
#define BUFFER_SIZE 4096

void error(string msg) {
  perror(msg.c_str());
  exit(1);
}

/*
    Responsible for processing user input, storing it in a char array. This function should throw
    an error if the user (1) doesn't enter any text or (2) enters text that exceeds 256 characters.
    Design decisions was to include the newline character as part of the 256 character limit, so 
    technically the user has 255 characters pluse the carriage return.
*/
char* user_input(char* buffer) {
  char c;
  int count = 0;

  printf("Enter your text message (256 character limit): ");
  c = getchar();
  while((c != '\n')) {
    if (count >= MAX_INPUT - 1) {
      error("ERROR: text message greater than 256 characters");
    }
    buffer[count++] = c;
    c = getchar();
  }

  if (!count) {
    error("ERROR: no valid text message provided");
  }
  return buffer;
}

/*
    This function creates a socket that will be used to communicate with the server machine. 
    It requires two arguments; host which the ip address of the server machine, and port which
    is the port number that the server socket is on. After successfully creating the socket, it then
    binds that socket to a specific address (ip address = client default addr. & port = 5433). After
    a successful bind, the user prompted to enter a message that is then sent to the server. If the 
    message is successfully sent, it waits for a response from the server. If a response is received 
    error free, then it gets displayed on the console.
*/
void communication(char* host, int port)
{
  int yes = 1;
  int sockfd;
  struct sockaddr_in cli_addr;
  struct sockaddr_in serv_addr;
  socklen_t addr_size;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    error("ERROR: creating an endpoint for communication");
  }

  memset(&cli_addr, 0, sizeof(cli_addr));
  cli_addr.sin_family = AF_INET;
  cli_addr.sin_port = htons(CLIENT_PORT);
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  inet_pton(AF_INET, host, &(serv_addr.sin_addr));
  addr_size = sizeof(serv_addr);

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    error("ERROR: refreshing socket"); 
  }

  if (bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
    error("ERROR: binding name to socket");
  }

  char buffer[MAX_INPUT];
  memset(buffer, 0, sizeof(buffer));
  char* msg = user_input(buffer);
  int n = sendto(sockfd, msg, strlen(msg) , 0, (struct sockaddr *)&serv_addr, addr_size);
  if ( n < 0)
  {
    error("ERROR: sending a message on the socket");
  }

  char resp[BUFFER_SIZE];
  memset(resp, 0, sizeof(resp));
  n = recvfrom(sockfd, resp, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addr_size);
  if (n < 0) {
    error("ERROR: receiving a message from socket");
  }

  printf("%s",resp);
  close(sockfd);
}

/*
    Parses the command line arguments. The user can pass in one type of
    arguments; (1) server's IP address If no IP is provided then it will 
    use the default ip address, which is based on my test environment.
    It will then call communication() passing in the appropriate ip and port.
*/
int main(int argc, char *argv[])
{
  int c;
  string svalue = "";
  while ((c = getopt(argc, argv, "s:h")) != -1) {
    switch (c) {
      case 's':
        svalue = optarg;
        break;
      case 'h':
        printf("usage:\n"          
        "   -s   with argument: server ip address (default ip = 192.168.1.217)\n"
        "   -h   Help Menu\n");
        exit(0);
    }
  }

  int port = SERVER_PORT;
  if (svalue.empty())
  {
    svalue = DEFAULT_IP;
  }

  char * host = new char[svalue.size() + 1];
  copy(svalue.begin(), svalue.end(), host);
  host[svalue.size()] = '\0';
  printf("Server's ip address: %s and port: %d\n", host, port);
  communication(host, port);
  
  return 0;
}