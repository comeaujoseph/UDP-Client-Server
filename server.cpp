//SERVER.CPP
#include <sys/types.h>    
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>

using namespace std;

#define DEFAULT_PORT 5432
#define SENDING_PORT 5433
#define BUFFER_SIZE 4096
#define MAX_LINE_SIZE 128
#define FILE_LENGTH 1028
#define FILEPATH "./liners"

void error(string msg) {
  perror(msg.c_str());
  exit(1);
}

/*
  This function is responsible for retrieving a random 
  saying from the file "liners". To do this it reads all 
  the lines from the file, storing them in a string array.
  It then generates a random index, which will be the index 
  of the funny saying. It then returns the random saying.
*/
string read_file()
{
  int length = 0;
  FILE *file = fopen (FILEPATH, "r" );
  string temp = "";
  if ( file != NULL )
  {
    string sentences[FILE_LENGTH];
    char line[MAX_LINE_SIZE];
    while ( fgets ( line, sizeof line , file ) != NULL )
    {
      string tmp(line);
      sentences[length] = tmp;
      length++;
    }
    srand(time(NULL));
    int random = (rand() % length);
    temp = sentences[random];
    fclose ( file );
  } else 
  {
    error("ERROR: file did not open properly");
  }
  return temp;
}

/*
  Takes one argument, char *msg, which points the the message sent 
  by the client. Calls read_file(), which retrieves a funny saying, 
  and then appends this saying to msg. Lastly, it returns this 
  new message.
*/
string liner_appender(char* msg)
{
  
  string line = read_file();
  string og(msg);
  string resp = og + " - " + line;

  return resp;
}

/*
  This function handles incoming and outgoing messages from the client 
  machine. It calls recvfrom() which blocks until it gets data from the client, 
  storing it in a char array. Once the data is received, the server will
  call line_appender() to append a funny saying. It will then use sendto()
  to send the new message back to the client. 
*/
void data_transfer(int sockfd)
{
  struct sockaddr_in cli_addr;
  socklen_t addr_size;
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, sizeof(buffer));
  addr_size = sizeof(cli_addr);

  int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cli_addr, &addr_size);
  if (n < 0) {
    error("ERROR: receiving a message from socket");
  }

  string concate = liner_appender(buffer);
  char * resp = new char[concate.size() + 1];
  copy(concate.begin(), concate.end(), resp);
  resp[concate.size()] = '\0';

  if ((sendto(sockfd, resp, strlen(resp), 0, (struct sockaddr *)&cli_addr, addr_size)) < 0)
  {
    error("ERROR: sending a message on the socket");
  }
}

/*
  This function creates a socket that will be used to communicate with the client machine.
  It then binds this socket to a specific address, which consists of an ip address and a
  port number. It then calls data_transfer() which handles receiving and sending data. 
*/
void communication(int port)
{
  int yes = 1;

  int sockfd;
  struct sockaddr_in serv_addr;

 
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    error("ERROR: creating an endpoint for communication");
  }
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    error("ERROR: refreshing socket"); 
  }

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    error("ERROR: binding name to socket");
  }

  data_transfer(sockfd);
  close(sockfd);
}

/*
  Simply calls communication() passing the default port number
  which is 5432.
*/
int main(int argc, char* argv[])
{
  int port = DEFAULT_PORT;

  communication(port);
  return 0;
}