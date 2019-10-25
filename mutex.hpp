/* 
    File: mutex.hpp

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 09/23/19

*/

#ifndef _mutex_H_                   // include file only once
#define _mutex_H_
#include <iostream>
/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <pthread.h>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   M u t e x  */
/*--------------------------------------------------------------------------*/

class Mutex {
  friend class MutexGuard;
    private:
    /* -- INTERNAL DATA STRUCTURES
     You may need to change them to fit your implementation. */
    
    pthread_mutex_t m;
    
    public:
    
    /* -- CONSTRUCTOR/DESTRUCTOR */
    
    Mutex();
    
    
    
    
    
    ~Mutex();
    
    
    /* -- MUTEX OPERATIONS */
    
    void Lock();
              
    void Unlock();
};



#endif


