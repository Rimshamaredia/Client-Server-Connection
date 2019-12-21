//
//  semaphore.cpp
//  MP2
//
//  Created by Rimsha Maredia on 10/5/19.
//  Copyright © 2019 Rimsha Maredia. All rights reserved.
//

#include "semaphore.hpp"
#include "mutex.hpp"
Semaphore::Semaphore(int _val){
    //initializing
    
    c = PTHREAD_COND_INITIALIZER;
    m = PTHREAD_MUTEX_INITIALIZER;
    value = _val;
}

Semaphore::~Semaphore(){
  
        pthread_cond_destroy(&c);
        pthread_mutex_destroy(&m);
    
}
int Semaphore::P(){
   
    pthread_mutex_lock(&m);
     value--;
    while(value<0){
        pthread_cond_wait(&c,&m);
        
    }
   
    pthread_mutex_unlock(&m);
    
    
    return 1;
    
}
int Semaphore::V(){
    value++;
    pthread_mutex_lock(&m);
    if(value==0){
        pthread_cond_signal(&c);
        
    }

    pthread_mutex_unlock(&m);
   
    return 1 ;
}
    


