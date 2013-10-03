#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#ifndef PORTNO
#define PORTNO 7889
#endif
  
#ifndef MAXBUFFER
#define MAXBUFFER 64
#endif  
  
int main(int argc, char *argv[])
{
    int sockfd, camNo, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

  	if (( argc < 2 ) || ( strlen(argv[1]) > 1 )) {
      printf("Usage: %s [camNo 1-9]\r\n", argv[0]);
      return 0;
  	}
  
  	camNo = atoi(argv[1]);
  
  	if (( camNo < 1 ) || ( camNo > 9 )) {
      printf("Usage: %s [camNo 1-9]\r\n", argv[0]);
      return 0;
  	}
  
    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        exit(1);
    }
    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(PORTNO);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
    {
         perror("ERROR connecting");
         exit(1);
    }	
    /* Now ask for a message from the user, this message
    * will be read by server
    */
    printf("Adding timer for camera #%d without flags. ( This utility does not support setting flags )\r\n", camNo);
    bzero(buffer,256);
  	buffer[0] = 'A';
  	buffer[1] = camNo + 48;
  	buffer[2] = '0';
  	buffer[3] = '0';
  	buffer[4] = '0';
  	buffer[5] = '0';
  
    /* Send message to the server */
    n = write(sockfd,buffer, 6);
    if (n < 0) 
    {
         perror("ERROR writing to socket");
         exit(1);
    }
    /* Now read server response */
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
    {
         perror("ERROR reading from socket");
         exit(1);
    }
    printf("Response: %s\n",buffer);
  	close(sockfd);
    return 0;
}

        
        
        
        

        
        
        