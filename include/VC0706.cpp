#include "VC0706.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "wiringSerial.h"

VC0706::VC0706(void) {
  this -> frameptr = 0;
  this -> bufferLen = 0;
  this -> currentbaud = 0;  
  this -> serialNum = 0;
}
  
VC0706::VC0706(const char *devPath) {
  strcpy(this -> devPath, devPath);
  this -> frameptr = 0;
  this -> bufferLen = 0;
  this -> currentbaud = 0;  
  this -> serialNum = 0;
}

VC0706::VC0706(char *devPath) {
  strcpy(this -> devPath, devPath);
  this -> frameptr = 0;
  this -> bufferLen = 0;
  this -> currentbaud = 0;  
  this -> serialNum = 0;
}

void VC0706::setDevPath(char *newDevPath) {
  strcpy(this -> devPath, newDevPath);
}

uint8_t VC0706::connectionSerialNum(void) {
  return this -> serialNum;
}

void VC0706::setConnectionSerialNum(uint8_t newSerialNum) {
  this -> serialNum = newSerialNum;
}

boolean VC0706::connect(void) {
  return this -> connect(38400);
}

boolean VC0706::connect(int baud) {
    
  if (( baud != 9600 ) && ( baud != 19200 ) && ( baud != 38400 ) &&
      ( baud != 57600) && ( baud != 115200 )) return false;

  this -> frameptr = 0;
  this -> bufferLen = 0;
  this -> currentbaud = 0;  
  
  if ( (this -> sc = serialOpen(this -> devPath, baud)) != -1 )
    this -> currentbaud = baud;
  
  return this -> sc == -1 ? false : true;
  
}

void VC0706::disconnect(void) {
  this -> currentbaud = 0;
  serialFlush(this -> sc);
  serialClose(this -> sc);
}

void VC0706::flush(void) {
  serialFlush(this -> sc);
}

uint8_t VC0706::flushData(void) {
  return this -> readResponse(50);
}

int VC0706::baudrate(void) {
  return this -> currentbaud;
}

boolean VC0706::setBaudrate(int baud) {
  
  uint8_t args[3] = {0x01, 0x0D, 0xA6};
  
  if (( baud != 9600 ) && ( baud != 19200 ) && ( baud != 38400 ) &&
      ( baud != 57600) && ( baud != 115200 )) return false;
  
  switch ( baud ) {
    case 9600: { args[1] = 0xAE; args[2] = 0xC8; break; }
    case 19200: { args[1] = 0x56; args[2] = 0xE4; break; }
    case 38400: { args[1] = 0x2A; args[2] = 0xF2; break; }
    case 57600: { args[1] = 0x1C; args[2] = 0x4C; break; }
    default: { args[1] = 0x0D; args[2] = 0xA6; }
  }
    
  if ( ! this -> runCommand(0x24, args, 3, false))
    return false;
  
  serialClose( this -> sc );
  usleep(10);
  
  this -> frameptr = 0;
  this -> bufferLen = 0;
  this -> currentbaud = 0;  
  
  if (( this -> sc = serialOpen(this -> devPath, baud)) != -1 )
    this -> currentbaud = baud;
  
  return this -> sc == -1 ? false : true;  
  
}

boolean VC0706::genVersion(void) {
  uint8_t args[] = {0x0};
  
  return this -> runCommand(0x11, args, 0);
}

boolean VC0706::systemReset(void) {
  uint8_t args[] = {0x0};
    
  if ( !this -> runCommand(0x26, args, 0 ))
    return false;

  serialFlush(this -> sc);
  serialClose(this -> sc);
    
  this -> frameptr = 0;
  this -> bufferLen = 0;
  this -> currentbaud = 0;  
  this -> serialNum = 0;
  
  if ( (this -> sc = serialOpen(this -> devPath, 38400)) != -1 )
    this -> currentbaud = 38400;
  
  return this -> sc == -1 ? false : true;
}

boolean VC0706::setSerialNo(uint8_t newSerialNo) {
  uint8_t args[] = {newSerialNo};
  if ( !this -> runCommand(0x21, args, 1))
    return false;
    
  this -> serialNum = newSerialNo;
  return true;  
}

boolean VC0706::tvOut(boolean state) {
  uint8_t args[] = {0x01, 0x0};
    
  if ( state ) args[1] = 0x01;
  
  this -> sendCommand(0x44, args, 2);
  if ( this -> readResponse(200) < 5 ) {
    return false;
  }
  
  if ( !this -> verifyResponse(0x44))
    return false;
  
  return camerabuff[3] == 0x01 ? true : false;
  
}

boolean VC0706::setMotionDetectorEnabled(boolean state) {
  uint8_t args[] = { 0x0, 0x01, 0x01};
  if ( !this -> runCommand(0x42, args, 3))
    return false;
    
  args[0] = state ? 1 : 0;
  
  if ( !this -> runCommand(0x37, args, 1))
    return false; 

  if ( !state ) {
    args[0] = 0x0;
    args[0] = 0x1;
    args[0] = 0x0;
    if ( !this -> runCommand(0x42, args, 3))
      return false;
  }
  
  return true;
}

boolean VC0706::motionDetectorEnabled(boolean &state) {
  uint8_t args[] = {0x0};
      
  if ( !this -> runCommand(0x38, args, 0))
    return false;
  
  if (( !this -> verifyResponse(0x38)) || ( this -> bufferLen < 6 ))
      return false;

  state = this -> camerabuff[5] == 1 ? true : false;    
  return true;
}

boolean VC0706::motionDetectorStatus(void) {

  if ( this -> readResponse(200) != 5 )
    return false;
    
  if (( this -> verifyResponse(0x39)) && ( this->responseStatus() == 0 ))
    return true;
  
  return false;
}

uint8_t VC0706::getDownsize(void) {
  uint8_t args[] = {0x0};
  if ( !this -> runCommand(0x55, args, 0 ))
    return -1;
  
  return camerabuff[5];
}

boolean VC0706::setDownsize(uint8_t newsize) {
  uint8_t args[] = { newsize };
    
  return this -> runCommand(0x54, args, 1);
}

uint8_t VC0706::getCompression(void) {
  uint8_t args[] = {0x1, 0x1, 0x12, 0x04};
   
  if ( !this -> runCommand(0x30, args, 4))
    return -1;
  
  if ( this -> bufferLen < 6 )
    return -1;
    
  return this -> camerabuff[5];
}

boolean VC0706::setCompression(uint8_t c) {
  uint8_t args[] = {0x1, 0x1, 0x12, 0x04, c};
  return this -> runCommand(0x31, args, 5);
}

uint8_t VC0706::getImageSize(void) {
  uint8_t args[] = {0x4, 0x1, 0x00, 0x19};
    
  if ( !this -> runCommand(0x30, args, 4))
    return -1;
    
  if ( this -> bufferLen < 6 )
    return -1;
    
  return this -> camerabuff[5];
}

boolean VC0706::setImageSize(uint8_t imagesize) {
  uint8_t args[] = {0x04, 0x01, 0x00, 0x19, imagesize};
    
  return this -> runCommand(0x31, args, 5);
}

boolean VC0706::freezeFrame(void) {
  uint8_t args[] = {0x0};
  frameptr = 0;
  
  return this -> runCommand(0x36, args, 1);  
}

boolean VC0706::freezeNextFrame(void) {
  uint8_t args[] = {0x1};
  frameptr = 0;
  
  return this -> runCommand(0x36, args, 1);  
}

boolean VC0706::stepFrame(void) {
  uint8_t args[] = {0x2};
  frameptr = 0;
  
  return this -> runCommand(0x36, args, 1);  
}

boolean VC0706::resumeVideo(void) {
  uint8_t args[] = {0x3};
  
  return this -> runCommand(0x36, args, 1);
}

uint32_t VC0706::fileSize(void) {
  uint8_t args[] = {0x00};
    
  if ( !this -> runCommand(0x34, args, 1))
    return 0;
    
  if ( this -> bufferLen < 9 )
    return 0;
    
  uint32_t len;
  len = this -> camerabuff[5];
  len <<= 8;
  len |= this -> camerabuff[6];
  len <<= 8;
  len |= this -> camerabuff[7];
  len <<= 8;
  len |= this -> camerabuff[8];
  
  return len;
}

uint8_t *VC0706::readBytes(uint8_t n) {
  uint8_t args[] = {0x0, 0x0A, 0x0, 0x0, frameptr >> 8, frameptr & 0xFF,
    0x0, 0x0, 0x0, n, CAMERADELAY >> 8, CAMERADELAY & 0xFF};
    
  if ( !this -> runCommand(0x32, args, 12, false))
    return 0;
    
  if ( this -> readResponse(CAMERADELAY, n + 5) == 0 )
    return 0;
    
  this -> frameptr += n;
  
  return this -> camerabuff;
}

boolean VC0706::saveImage(const char *filename) {
  FILE *fd = NULL;
    
  if (( fd = fopen(filename, "w")) == NULL )
    return false;
  
  uint32_t jpglen = this -> fileSize();
  while ( jpglen > 0 ) {
    uint8_t *buffer;
    uint8_t bytesToRead = jpglen > VC0706_BLOCKSIZE ? VC0706_BLOCKSIZE : jpglen;
    
    buffer = this -> readBytes(bytesToRead);
    fwrite(buffer, bytesToRead, 1, fd);
    jpglen -= bytesToRead;
    
  }
  fclose(fd);
  return true;
}


boolean VC0706::saveImage(char *filename) {
  FILE *fd = NULL;
    
  if (( fd = fopen(filename, "w")) == NULL )
    return false;
  
  uint32_t jpglen = this -> fileSize();
  while ( jpglen > 0 ) {
    uint8_t *buffer;
    uint8_t bytesToRead = jpglen > VC0706_BLOCKSIZE ? VC0706_BLOCKSIZE : jpglen;
    
    buffer = this -> readBytes(bytesToRead);
    fwrite(buffer, bytesToRead, 1, fd);
    jpglen -= bytesToRead;
    
  }
  fclose(fd);
  return true;
}

boolean VC0706::runCommand(uint8_t cmd, uint8_t *args, uint8_t argn, boolean flushflag) {

  if ( flushflag ) {
    this -> readResponse(50);
  }

  this -> sendCommand(cmd, args, argn);
  if ( this -> readResponse(200) < 5 ) {
    return false;
  }
  
  if ( !this -> verifyResponse(cmd))
    return false;
  
  if ( this -> responseStatus() != 0x0 )
    return false;
  
  return true;
}

void VC0706::sendCommand(uint8_t cmd, uint8_t args[] = 0, uint8_t argn = 0) {
	  
  serialPutchar(this -> sc, 0x56);
  serialPutchar(this -> sc, this -> serialNum);
  serialPutchar(this -> sc, cmd);
  serialPutchar(this -> sc, argn);
  
  for ( uint8_t i=0; i < argn; i++ ) {
    serialPutchar(this -> sc, args[i]);
  }
    
}

boolean VC0706::readResponse(uint8_t timeout, const uint8_t maxbytes) {
  uint8_t counter = 0;
  this -> bufferLen = 0;
  int avail, ch;
  uint8_t numbytes = maxbytes != 0 ? maxbytes : 5;
  
  while (( timeout != counter ) && ( this -> bufferLen != numbytes )) {
    if (( avail = serialDataAvail( this -> sc )) < 1 ) {
      usleep(1000);
      counter++;
      continue;
    }
    
    counter = 0;
    ch = serialGetchar( this -> sc );
    if (( maxbytes == 0 ) && ( this -> bufferLen == 4 ))
      numbytes += ch;
    
    this -> camerabuff[this -> bufferLen++] = ch;
    
  }
  return bufferLen;
}

char *VC0706::responseData(void) {
  if ( this -> bufferLen < 6 ) return 0;
  camerabuff[this -> bufferLen] = 0;
  return (char *)this -> camerabuff + 5;
}

uint8_t VC0706::responseDataLength(void) {
  return this -> camerabuff[4];
}

uint8_t VC0706::responseStatus(void) {
  return this -> camerabuff[3];
}

boolean VC0706::verifyResponse(uint8_t command) {

  // Check for valid protocol
  if (( this -> camerabuff[0] != 0x76 ) || 
      ( this -> camerabuff[1] != this -> serialNum ) || 
      ( this -> camerabuff[2] != command )) return false;
  
  // Check for valid answer from camera
  if (( this -> camerabuff[3] == 0x0 ) || // OK
      ( this -> camerabuff[3] == 0x1 ) || // Did not receive command
      ( this -> camerabuff[3] == 0x2 ) || // incorrect data-length
      ( this -> camerabuff[3] == 0x3 ) || // incorrect data format
      ( this -> camerabuff[3] == 0x4 ) || // busy - can't do this now
      ( this -> camerabuff[3] == 0x5 )) // Received OK, but operation failed
    return true;
  
  return false;
}

void VC0706::printBuff(void) {
  for ( uint8_t i = 0; i < this -> bufferLen; i++ ) {
    printf(" 0x%x", this->camerabuff[i]);
  }
  
  printf("\r\n");
}
