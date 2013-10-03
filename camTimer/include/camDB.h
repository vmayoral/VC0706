#ifndef _CAM_DB_H_
#define _CAM_DB_H_ 1

#include <time.h>
#include <stdlib.h>

#ifndef TIMESTAMP_INTERVAL  
#define TIMESTAMP_INTERVAL 15  
#endif
  
class cTimerDB {
  public:

  	cTimerDB(void); // Constructor
  	~cTimerDB(void); // Destructor

	// initialize with numCams amount of cameras.. 0 = fail, 1 = OK
	int createDB(unsigned char numCams); 
	// Destroys all timers and camera DBs..
	void destroyDB(void); 
	// Adds a timer for camera #camNo with flags, or updates flags and time for existing timer.
	int addTimer(unsigned char camNo, unsigned char flagP, unsigned char flagD, unsigned char flagS, unsigned char flagU);
	// Returns id of timer if timer is allowed to execute and exists for camera #camNo.
	unsigned char timerAvailable(unsigned char camNo);
	// Removes timer with id timerId on camera #camNo, and sets flag variables..
	int timerDone(unsigned char camNo, unsigned char timerId, 
              unsigned char &flagP, unsigned char &flagD, unsigned char &flagS, unsigned char &flagU);
  
  	// Returns number of cameras in db
  	unsigned char amountCams(void);
  
  	// Return amount of timers for camera #camNo in db
  	unsigned char amountTimers(unsigned char camNo);
  
  private:
	struct timer_struct {
      unsigned char id; // Id of request
      time_t timestamp; // Minimum timestamp before execution allowed
      unsigned char flags[4]; // 0 = panel, 1 = doorbell, 2 = sensor, 3 = user
      struct timer_struct *next;
    };
  
    struct cam_struct {
      unsigned char camID; // camera number
      unsigned char nextId; // Stores next ID
      struct timer_struct *timers; // Holds pending timers..
      struct cam_struct *next;
	} *camDB;

  	unsigned char _amountCams;
  	unsigned char _amountTimers[10];
  
};
  
#endif
        
        
        