#include "../settings.h"
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

void usage(char *appPath) {
	printf("Give [1-3] as parameter for application to set camera resolution.\r\n");
	printf("example: %s 2\r\n\n", appPath);
	printf("	1 - 160x120\r\n");
	printf("	2 - 320x240\r\n");
	printf("	3 - 640x480\r\n\n");
}

int main(int argc, char *argv[]) {

  if ( argc != 2 ) {
    usage(argv[0]);
    return 0;
  }
  
  if (( argv[1][0] != '1' ) &&
      ( argv[1][0] != '2' ) &&
      ( argv[1][0] != '3' )) {
    usage(argv[0]);
    return 0;
  }
  
  
  uint8_t size = 0;
  
  if ( argv[1][0] == '1' ) size = SIZE_160x120;
  if ( argv[1][0] == '2' ) size = SIZE_320x240;
  if ( argv[1][0] == '3' ) size = SIZE_640x480;
	
  if ( !initiateConnection()) {
    printf("Abort.\r\n");
    camera.disconnect();
    return 0;
  }

  	printf("Connected to camera. Operating with %d baudrate.\r\n", camera.baudrate());         
	char *reply = camera.responseData();
	printf("Version: \"%s\"\r\n", reply);
         
  
  printf("Setting image size to: ");
  if ( argv[1][0] == '1' ) printf("160x120\r\n");
  if ( argv[1][0] == '2' ) printf("320x240\r\n");
  if ( argv[1][0] == '3' ) printf("640x480\r\n");

  if ( !camera.setImageSize(size)) printf("Failed to set camera resolution.\r\n");
  else {
	printf("Should be set now - use cameraInfo. Then use fetch and check image's resolution.\r\n");
	printf("Propably fetched image has wrong resolution, reset camera and you should be all set..\r\n");
  }

  camera.disconnect();
  return 0;
}

        
        
        

        
