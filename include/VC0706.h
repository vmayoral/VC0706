/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	VC0706 library v1.1 - 23.9.2013
	Written by Oskari Rauta
	License: expat ( check copying.txt )

	Partially based on Adafruit's VC0706 library, but this is
	a complete rewrite which also supports using higher baudrates
	like 115200. Check usage for higher rates from samples in
	src directory - function is initiateConnection();

	CREDITS: I didn't write serial library - I used existing
	one from wiringPI project. Thank you for great work.
	
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  
#ifndef VC0706_h_
#define VC0706_h_ 1

#include "util.h"
  
// Don't use less than 100 as camerabuffsize and blocksize is always atleast 21 less than buffsize.  
// And don't play with cameradelay too, it isn't there to slow down camera - it's there to tolerate camera slowliness.
#define CAMERABUFFSIZE 220
#define CAMERADELAY 10
#define VC0706_BLOCKSIZE 192

// Size codes.. Use when setting/getting camera resolution
#define SIZE_640x480 0x00
#define SIZE_320x240 0x11
#define SIZE_160x120 0x22

/*
	Return values: (from responseStatus())
	0: OK
	1: Camera did not receive the command
	2: Data-length error
	3: Data-format error
	4: Command cannot execute right now. Busy. ( Trying to save image without freezing the frame?)
	5: Command received, but something went wrong as it didn't execute the right way.
*/
  
class VC0706 {
  public:
  	VC0706(void); // Constructor - if this is used, use setDevPath later
  	VC0706(const char *devPath); // Constructor with devPath
  	VC0706(char *devPath); // Constructor with devPath from variable
  	~VC0706(); // Destructor
  	void setDevPath(char *newDevPath); // sets the device, only needed if you didn't construct with devpath
  	void setDevPath(const char *newDevPath); // Same as previous - just with const..
  	uint8_t connectionSerialNum(void); // Current connection's serial number ( always initially 0 )
  	void setConnectionSerialNum(uint8_t newSerialNum); // If you have exited your program after changing serial number, and did not recycle camera's power, or reset it - serial number is going to stick.. So use this after constructing.
  
  	boolean connect(void); // Connect. Without arguments, use baudrate 38400.
  	boolean connect(int baud); // Connect - available baud rates are: 9600, 19200, 38400, 57600 and 115200 - after power up, baudrate is always 38400.
  	void disconnect(void); // Disconnects serial connection.
  	void flush(void); // Flushes outgoing serial. - Didn't need this, but added it anyway.
  	uint8_t flushData(void); // Flushes incoming serial
  	int baudrate(void); // Currently used baudrate, or 0 if connection was terminated due to error - or something else is wrong.
  
  	char *responseData(void); // If data contains a string, like.. Version? This would return it..
  	uint8_t responseDataLength(void); // Length of previously mentioned string
  	uint8_t responseStatus(void); // Status byte of response
  
  	boolean setBaudrate(int baud); // Set baudrate. Baudrates mentioned earlier are accepted. This also re-connects with new baudrate.
  	boolean genVersion(void); // Request version. Version ID can be retrieved from responseData();

  	boolean systemReset(void); // Resets the system with some variables. Also if baudrate is not 38400, this sets baud back to 38400 and reconnects. Camera serial number is also reset. This is needed if you want to change image size in fb..
  	boolean setSerialNo(uint8_t newSerialNo); // Sets new serial number. Also for the protocol. No other commands needed after the change.
  	boolean tvOut(boolean state); // Set tvOut to on(true) or off(false)..
  	boolean setMotionDetectorEnabled(boolean state); // enable or disable motion detector. Initially disabled. Setting stays if not reset, or power cycled.
  	boolean motionDetectorEnabled(boolean &state); // check if motion detector is enabled or not. Result is stored to state, function returns false if failed to query this.
  	boolean motionDetectorStatus(void); // Returns true, if motiondetector is enabled and motion is detected. This does not check if motiondetector is enabled, so it's up to you.
  
  	uint8_t getDownsize(void);
  	boolean setDownsize(uint8_t newsize);
  	uint8_t getCompression(void); // compression level or -1 on failure
  	boolean setCompression(uint8_t c); // set compression level
  	uint8_t getImageSize(void); // image size ( check 3 sizes from defines of this file ) or -1 on failure
  	boolean setImageSize(uint8_t imagesize); // set image size - might need a reset to camera to work.

  	boolean freezeFrame(void); // Freeze camera's current frame for image downloading
  	boolean freezeNextFrame(void); // Freeze next frame for image..
  	boolean stepFrame(void); // Step frame..
  	boolean resumeVideo(void); // Resume video..
  	uint32_t fileSize(void); // Returns fileSize for image file for freezed frame
  	uint8_t *readBytes(uint8_t n); // Read n bytes of this file--
  	boolean saveImage(const char *filename); // Save's frozen frame to filename
  	boolean saveImage(char *filename); // Same --
  
  private:
  	char *devPath;
  	int sc;
  	uint8_t camerabuff[CAMERABUFFSIZE+1];
  	uint8_t bufferLen;
  	uint32_t frameptr;
  	uint8_t serialNum;
  	int currentbaud;
  
  	boolean runCommand(uint8_t cmd, uint8_t args[], uint8_t argn, boolean flushflag = false);
  	void sendCommand(uint8_t cmd, uint8_t args[], uint8_t argn);
  	uint8_t readResponse(uint8_t timeout, const uint8_t maxbytes = 0);
  	boolean verifyResponse(uint8_t command);
  	void printBuff(void);
};
  
#endif

        
        
        
        

        
