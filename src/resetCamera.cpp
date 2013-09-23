#include "../settings.h"
#include "../include/util.h"
#include "../include/VC0706.h"
#include <stdio.h>
#include <unistd.h>

VC0706 camera(VC0706DEV);

boolean initiateConnection() {  
  
  if ( !camera.connect(BAUDRATE)) {
    printf("Cannot connect to serial port[baud: %d]\r\n", BAUDRATE);
    return false;
  }

  if ( !camera.genVersion()) {
    if ( BAUDRATE == 38400 ) {
      printf("Cannot communicate with camera.\r\n");
      return false;
    }
		
    printf("Baudrate %d not set. Trying to set it.\r\n", BAUDRATE);
    camera.disconnect();
    usleep(10);
		
    if ( !camera.connect(38400)) {
      printf("Cannot connect to serial port[baud: 38400]\r\n");
      return false;
    }
		
    if ( !camera.genVersion()) { printf("Failure while communicating with camera.\r\n"); return false; }
    if ( !camera.setBaudrate(BAUDRATE)) { printf("Cannot set baudrate to %d.\r\n", BAUDRATE); return false; }
    if ( !camera.genVersion()) { printf("Failed to initialize camera with baudrate %d.\r\n", BAUDRATE); return false;}
  }
  return true;
}


int main() {

	if ( !initiateConnection()) {
		printf("Abort.\r\n");
		camera.disconnect();
		return 0;
	}

	printf("Connected to camera. Operating with %d baudrate.\r\n", camera.baudrate());
  
  if ( !camera.systemReset()) {
    printf("Failed to reset camera.\r\n");
    camera.disconnect();
    return 0;
  }

  /* Reset message should be:
3 1.00
Ctrl infr exist
USER-defined sensor
525
Init end
*/
  
  printf("Camera has been reset. Reset message follows: \r\n%s\r\n",
	camera.responseData());
	
  camera.disconnect();
  return 0;
}

        
        

        

        
