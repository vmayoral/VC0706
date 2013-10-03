#include "camDB.h"

cTimerDB::cTimerDB(void) {
  this -> camDB = NULL;
  this -> _amountCams = 0;
  this -> _amountTimers[0] = this -> _amountTimers[1] = this -> _amountTimers[2] = 
  this -> _amountTimers[3] = this -> _amountTimers[4] = this -> _amountTimers[5] = 
  this -> _amountTimers[6] = this -> _amountTimers[7] = this -> _amountTimers[8] =
  this -> _amountTimers[9] = 0;
}

cTimerDB::~cTimerDB() {
  this -> destroyDB();
}
  
int cTimerDB::createDB(unsigned char numCams) {
  
  if ( this -> camDB != NULL )
    this -> destroyDB();
  
  struct cam_struct *ptr = ( struct cam_struct*)malloc(sizeof(struct cam_struct));
  if ( ptr == NULL ) {
    return 0;
  }
  
  ptr -> camID = 1;
  ptr -> nextId = 1;
  ptr -> timers = NULL;
  ptr -> next = NULL;
  this -> camDB = ptr;
  if ( numCams > 1 )
    for ( unsigned char t = 1 ; t < numCams; t++ ) {
      ptr -> next = ( struct cam_struct*)malloc(sizeof(struct cam_struct));
      if ( ptr -> next == NULL ) {
        this -> destroyDB();
        return 0;
      }
      ptr -> next -> camID = t + 2;
      ptr -> next -> nextId = 1;
      ptr -> timers = NULL;
      ptr -> next -> next = NULL;
      ptr = ptr -> next;
    }

  this -> _amountCams = numCams;
  
  return 1;
}

void cTimerDB::destroyDB(void) {
  
  if ( this -> camDB == NULL )
    return;
  
  struct cam_struct *ptr;
  
  while ( this -> camDB -> next != NULL ) {
    ptr = this -> camDB -> next;
    this -> camDB -> next = ptr -> next;
    
    struct timer_struct *ptr2;
    if ( ptr -> timers != NULL ) {
      while ( ptr -> timers -> next != NULL ) {
        ptr2 = ptr -> timers -> next;
        ptr -> timers -> next = ptr2 -> next;
        free(ptr2);
        ptr2 = NULL;
      }
      free( ptr -> timers );
      ptr -> timers = NULL;
    }
    free( ptr );
    ptr = NULL;
  }
  free( this -> camDB );
  this -> camDB = NULL;  

  this -> _amountCams = 0;
  this -> _amountTimers[0] = this -> _amountTimers[1] = this -> _amountTimers[2] = 
  this -> _amountTimers[3] = this -> _amountTimers[4] = this -> _amountTimers[5] = 
  this -> _amountTimers[6] = this -> _amountTimers[7] = this -> _amountTimers[8] =
  this -> _amountTimers[9] = 0;
  
}

int cTimerDB::addTimer(unsigned char camNo, unsigned char flagP, unsigned char flagD, unsigned char flagS, unsigned char flagU) {

  struct cam_struct *ptr = this -> camDB;
  
  while ( ptr != NULL ) {
    if ( ptr -> camID == camNo ) {
      if (( ptr -> timers != NULL ) && (( flagP > 0 ) || ( flagD > 0 ) || ( flagS > 0 ) || ( flagU > 0 ))) {
        ptr -> timers -> timestamp = time(NULL) - TIMESTAMP_INTERVAL;
        if ( flagP > 0 ) ptr -> timers -> flags[0] = flagP;
        if ( flagD > 0 ) ptr -> timers -> flags[1] = flagD;
        if ( flagS > 0 ) ptr -> timers -> flags[2] = flagS;
        if ( flagU > 0 ) ptr -> timers -> flags[3] = flagU;
        return 2;
      }
      
      struct timer_struct *ptr2;
      time_t nextT = time(NULL) - TIMESTAMP_INTERVAL;
      if ( ptr -> timers == NULL ) {
  		ptr -> timers = ( struct timer_struct*)malloc(sizeof(struct timer_struct));        
        ptr2 = ptr -> timers;
        if ( ptr2 == NULL ) return 0;
      } else {
        ptr2 = ptr -> timers;
        nextT = ptr2 -> timestamp + TIMESTAMP_INTERVAL;
        while ( ptr2 -> next != NULL ) {
          nextT = ptr2 -> timestamp + TIMESTAMP_INTERVAL;
          ptr2 = ptr2 -> next;
        }
  		ptr2 -> next = ( struct timer_struct*)malloc(sizeof(struct timer_struct));
        ptr2 = ptr2 -> next;
        if ( ptr2 == NULL ) return 0;
      }
      
      if ( ptr -> nextId == 0 ) ptr -> nextId = 1;
      ptr2 -> id = ptr -> nextId;
      ptr -> nextId++;
      if (( flagP > 0 ) || ( flagD > 0 ) || ( flagS > 0 ) || ( flagU > 0 )) ptr2 -> timestamp = time(NULL) - TIMESTAMP_INTERVAL;
      else ptr2 -> timestamp = nextT; // 15 seconds later than last request..
      ptr2 -> flags[0] = flagP;
      ptr2 -> flags[1] = flagD;
      ptr2 -> flags[2] = flagS;
      ptr2 -> flags[3] = flagU;
      ptr2 -> next = NULL;
      
      this -> _amountTimers[camNo - 1]++;
      
      return 1;
    }
    ptr = ptr -> next;
  }
  
  return 0;  
}
  
unsigned char cTimerDB::timerAvailable(unsigned char camNo) {
  struct cam_struct *ptr = this -> camDB;
  time_t now = time(NULL);

  while ( ptr != NULL ) {
    
    if ( ptr -> camID == camNo ) {
      if ( ptr -> timers == NULL )
        return 0;
      
      return ptr -> timers -> timestamp < now ? ptr -> timers -> id : 0;
    }
    
    ptr = ptr -> next;
  }
  
  return 0;
}

int cTimerDB::timerDone(unsigned char camNo, unsigned char timerId, unsigned char &flagP,
               unsigned char &flagD, unsigned char &flagS, unsigned char &flagU) {
  struct cam_struct *ptr = this -> camDB;
  
  while ( ptr != NULL ) {
    
    if ( ptr -> camID == camNo ) {
      struct timer_struct *ptr2 = ptr -> timers;
      if ( ptr2 -> id == timerId ) {
        flagP = ptr2 -> flags[0];
        flagD = ptr2 -> flags[1];
        flagS = ptr2 -> flags[2];
        flagU = ptr2 -> flags[3];
        ptr -> timers = ptr2 -> next;
        ptr2 -> next = NULL;
        free(ptr2);
        ptr2 = NULL;
      	this -> _amountTimers[camNo - 1]--;
        return 1;
      } else {
        struct timer_struct *prev = ptr -> timers;
        ptr2 = prev -> next;
        while ( ptr2 != NULL ) {
      
          if ( ptr2 -> id == timerId ) {
            flagP = ptr2 -> flags[0];
            flagD = ptr2 -> flags[1];
            flagS = ptr2 -> flags[2];
            flagU = ptr2 -> flags[3];
            prev -> next = ptr2 -> next;
            ptr2 -> next = NULL;
            free(ptr2);
            ptr2 = NULL;        
      		this -> _amountTimers[camNo - 1]--;
            return 1;
          }
        
          prev = ptr2;
          ptr2 = ptr2 -> next;
      
        }
      }
    }
    
    ptr = ptr -> next;
  }
  return 0;
}

unsigned char cTimerDB::amountCams(void) {
  return this -> _amountCams;
}

unsigned char cTimerDB::amountTimers(unsigned char camNo) {
  if ( camNo > this -> _amountCams )
    return 0;
  
  return this -> _amountTimers[camNo - 1];
}

        
        
        