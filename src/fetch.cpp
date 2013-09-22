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


int main() {

	if ( !initiateConnection()) {
		printf("Abort.\r\n");
		camera.disconnect();
		return 0;
	}

	printf("Connected to camera. Operating with %d baudrate.\r\n", camera.baudrate());
	char *reply = camera.responseData();
	printf("Version: \"%s\"\r\n", reply);
	printf("Image-size: ");
	uint8_t s = camera.getImageSize();
	if ( s == SIZE_640x480 ) printf("640x480\r\n");
	else if ( s == SIZE_320x240 ) printf("320x240\r\n");
	else if ( s == SIZE_160x120 ) printf("160x120\r\n");
	else printf("Error!\r\nCannot retrieve framebuffer image's resolution.\r\n");
      
    printf("\r\n * * * WARNING * * * \r\n");
    printf("Setting framebuffer resolution seems to be tricky. It can be set after\r\n");
    printf("camera has been reset. If one sets it later, selected resolution is reported\r\n");
    printf("by camera - but fetched images will be with set resolution - so information here\r\n");
  	printf("isn't absolute - check resolution of fetched image to be sure.\r\n\n");

	printf("Taking the picture now..\r\n\n");

	if ( !camera.freezeFrame()) printf("Cannot freeze frame..\r\n");
	else {
	uint32_t fs = camera.fileSize();
	printf("Size of jpeg from currently frozen frame is : %d bytes.\r\n", fs);
	printf("Writing frozen frame to %s\r\n", FETCHFILE);
	camera.saveImage(FETCHFILE);
	printf("And done..\r\n");
	}
	if ( !camera.resumeVideo()) printf("Video cannot resume..\r\n");

	camera.disconnect();
	return 0;
}

        
        
        
