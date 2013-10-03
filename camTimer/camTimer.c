
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include "include/camDB.h"

#ifndef PORTNO
#define PORTNO 7889
#endif
  
#ifndef MAXBUFFER
#define MAXBUFFER 64
#endif
  
#ifndef MAXTIMERS
#define MAXTIMERS 20
#endif
  
cTimerDB camDB;
int exitServer = 0;

void usage(char *str) {
  printf("\r\nUsage: %s [number of cameras]\r\n", str);
  printf("for example: %s 2\r\n\n", str);
  printf("[number of cameras] must be a number from 1 to 9.\r\n");
  printf("Example command would start timers for 2 cameras.\r\n\n");
}


void doprocessing (int sock)
{
  char buffer[MAXBUFFER];
  char id[10];
  char flags[12];
  bzero(buffer, MAXBUFFER);
  int ret, n = read(sock, buffer, MAXBUFFER-1);
  unsigned char fP, fD, fS, fU;
  
  if (n < 0) {
    printf("Socket read error.\r\n");
    exit(1);
  }

  if (( buffer[2] == 0 ) && ( buffer[0] == 'X' ) && ( buffer[1] == 'X' )) { // Exit 
    if ( write(sock, "Quit", 4) < 0 ) {
      exitServer = 1;
      printf("Socket write error.\r\n"); 
      exit(1); 
    }
    exitServer = 1;
  }  
  
  if (( buffer[2] == 0 ) && ( buffer[0] == 'Q') &&
      ( buffer[1] > 48 ) && ( buffer[1] < 58 )) { // Query

    if ( camDB.amountCams() < ( buffer[1] - 48 ))
      ret = 0;
    else    
      ret = camDB.timerAvailable( buffer[1] - 48 );
    
    bzero(id, 12);
    sprintf(id, "ID %03d", ret);
          
    if ( write(sock, id, 6) < 0 ) {
      printf("Socket write error.\r\n"); 
      exit(1); 
    }
    
  }
  
  if (( buffer[0] == 'A' ) && ( buffer[6] == 0 ) &&
      ( buffer[1] > 48 ) && ( buffer[1] < 58 ) &&
      ((( buffer[2] == '0' ) || ( buffer[2] == '1' )) &&
       (( buffer[3] == '0' ) || ( buffer[3] == '1' )) &&
       (( buffer[4] == '0' ) || ( buffer[4] == '1' )) &&
       (( buffer[5] == '0' ) || ( buffer[5] == '1' )))) { // Add query
    
    if ( camDB.amountCams() < ( buffer[1] - 48 ))
      ret = 4;
    else if ( camDB.amountTimers(buffer[1] - 48) >= MAXTIMERS )
      ret = 3;
    else
      ret = camDB.addTimer(buffer[1] - 48,
                           buffer[2] == '0' ? 0 : 1,
                           buffer[3] == '0' ? 0 : 1,
                           buffer[4] == '0' ? 0 : 1,
                           buffer[5] == '0' ? 0 : 1);
    
    if ( !ret ) {
      if ( write(sock, "Memory allocation error", 23) < 0 ) {
        printf("Socket write error.\r\n"); exit(1); }
    } else {
      
      switch ( ret ) {
        case 2: { // Updated
          if ( write(sock, "Update", 6) < 0 ) {
            printf("Socket write error.\r\n"); 
            exit(1); 
          }
          break;
        }
        case 3: { // Refused
          if ( write(sock, "List full", 9) < 0 ) {
            printf("Socket write error.\r\n"); 
            exit(1); 
          }
          break;
        }
        case 4: { // Camera not in db
          if ( write(sock, "Camera not in db", 16) < 0 ) {
            printf("Socket write error.\r\n"); 
            exit(1); 
          }
          break;          
        }
        default: { // 1 - OK
          if ( write(sock, "Added", 5) < 0 ) {
            printf("Socket write error.\r\n"); 
            exit(1); 
          }
          break;
        }
      }
    }
  }
    
  if (( buffer[0] == 'D') && ( buffer[5] == 0 ) &&
      ( buffer[1] > 48 ) && ( buffer[1] < 58 ) &&
      ( buffer[2] > 47 ) && ( buffer[2] < 58 ) &&
      ( buffer[3] > 47 ) && ( buffer[3] < 58 ) &&
      ( buffer[4] > 47 ) && ( buffer[4] < 58 )) { // Done

    if ( camDB.amountCams() < ( buffer[1] - 48 )) {
      if ( write(sock, "Camera not in db", 16) < 0 ) {
        printf("Socket write error.\r\n"); 
        exit(1); 
      }
      return;
    }
    
    fP = fD = fS = fU = 0;
    ret = camDB.timerDone(buffer[1] - 48, 
                          0 + (( buffer[2] - 48 ) * 100 ) + (( buffer[3] - 48 ) * 10 ) + ( buffer[4] - 48 ),
                          fP, fD, fS, fU);
    
    bzero(flags, 12);
    sprintf(flags, "FLAGS %d%d%d%d", fP, fD, fS, fU);
    
    if ( !ret ) {
      if ( write(sock, "Timer not found", 15) < 0 ) {
        printf("Socket write error.\r\n"); 
        exit(1); 
      }
    } else {
      if ( write(sock, flags, 10) < 0 ) {
        printf("Socket write error.\r\n"); 
        exit(1); 
      }
    }
    
  }
  
}


int main( int argc, char *argv[] ) {
  
  if (( argc == 2 ) && ( strlen(argv[1]) == 1 ) &&
      (argv[1][0] > 48 ) && ( argv[1][0] < 58 )) {
    if ( !camDB.createDB(argv[1][0] - 48) ) {
      printf("Memory allocation error.\r\n");
      exit(1);
    }
  } else {
    usage(argv[0]);
    return 0;
  }
      
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
  if ( sockfd < 0 ) {
    printf("Socket opening error.\r\n");
    exit(1);
  }

  struct sockaddr_in serv_addr;
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORTNO);
  

  if ( bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ) {
    printf("Binding error. (Another server already bind on port %d?)\r\n", PORTNO);
    exit(1);
  }  

  int on = 1;
 
  if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                  (const char *) &on, sizeof(on)) == -1 ) {
    printf("Socket option error. Socket attributes cannot be set.\r\n");
    exit(1);
  }
  
  if ( listen(sockfd, 5) < 0 ) {
    printf("Listening error. Socket cannot be listened.\r\n");
    exit(1);
  }
  
  struct sockaddr_in cli_addr;
  int clilen = sizeof(cli_addr);

  fd_set active_fd_set, read_fd_set;  
  FD_ZERO (&active_fd_set);
  FD_SET(sockfd, &active_fd_set);
  
//  int amountCons = 0;
  
  while ( !exitServer ) {
        
    read_fd_set = active_fd_set;

    if ( select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0 ) {
      printf("Error with select.\r\n");
      exit(1);
    }
         
    for ( int i = 0; i < FD_SETSIZE; ++i )
      if (( i == sockfd ) && ( FD_ISSET(i, &read_fd_set))) {
          
//        printf("Accepted connection #%d..\r\n", amountCons++);
          
        int newsockfd;
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);

        if ( newsockfd < 0 ) {
          printf("Error while accepting connection.\r\n");
          printf("Status code: %d. Error code: %d. Error description:\r\n%s\r\n", newsockfd, errno, strerror(errno));
          exit(1);
        }
        
        doprocessing(newsockfd);
        close(newsockfd);
        
      }
          
  } /* end of while */
 
  close(sockfd);
//  shutdown(sockfd, SHUT_RDWR);
  return 0;
  
}

        
        
        
        
        
        
        
        
        