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
	char *reply = camera.responseData();
	printf("Version: \"%s\"\r\n", reply);
	printf("Downsize: %d\r\n", camera.getDownsize());
	printf("Compression-level: %d\r\n", camera.getCompression());
	printf("Image-size: ");
	uint8_t s = camera.getImageSize();
	if ( s == SIZE_640x480 ) printf("640x480\r\n");
	else if ( s == SIZE_320x240 ) printf("320x240\r\n");
	else if ( s == SIZE_160x120 ) printf("160x120\r\n");
	else printf("Error!\r\nCannot retrieve framebuffer image's resolution.\r\n");
	boolean state = true;
	if ( !camera.motionDetectorEnabled(state)) {
		printf("Failed to get motion detector status.\r\n");
	} else printf("Motion detector state: %s\r\n", state ? "enabled" : "disabled");

	camera.disconnect();
	return 0;
}

        
        
