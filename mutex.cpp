#include "mutex.hpp"
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

Mutex::Mutex(){
    this->m = PTHREAD_MUTEX_INITIALIZER;
}

Mutex::~Mutex(){
    printf("Destroying mutex\n");
    pthread_mutex_destroy(&m);
}
void Mutex::Lock(){
    printf("Locking mutex\n");
    pthread_mutex_lock(&m);
    
}
void Mutex::Unlock(){
    printf("Unlocking the mutex\n");
    pthread_mutex_unlock(&m);
}
  
