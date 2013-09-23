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

	printf("Connected to camera. Operating with %d baudrate.\r\n\n", camera.baudrate());
	char *reply = camera.responseData();
	printf("Version: \"%s\"\r\n", reply);
  
	boolean state = true;
	if ( !camera.motionDetectorEnabled(state)) {
		printf("Failed to get motion detector status.\r\n");
	} else printf("Motion detector state: %s\r\n", state ? "enabled" : "disabled");
  
  	
	if ( camera.setMotionDetectorEnabled(true)) {
		printf("Enabling motion detector.\r\n");
		if ( !camera.motionDetectorEnabled(state))
			printf("Failed to get status.\r\n");
		else printf("Motion detector state is: %s\r\n", state ? "enabled" : "disabled");
	} else {
      printf("Failed to enable motion detector.\r\n");
      camera.disconnect();
      return 0;
    }

	int rr = 0;

	printf("Detecting motion now (move over camera 10 times to quit)..\r\n");
	while ( rr < 10 )
		if ( camera.motionDetectorStatus())
			printf("%d. time: motion detected.\r\n", rr++);

	if ( !camera.setMotionDetectorEnabled(false)) {
		printf("Failed to disable camera's motion detector.\r\n");
	} else {
		while ( camera.flushData() != 0 );
		if ( !camera.motionDetectorEnabled(state))
			printf("Failed to get camera's motion detector's state.\r\n");
		else printf("Motion detector status: %s\r\n", state ? "YES" : "NO");
	}

	camera.disconnect();
	return 0;
}

        
        

        
