#ifndef Boundedbuffer_hpp
#define Boundedbuffer_hpp

#include "mutex.hpp"
#include<string>

#include <stdio.h>
#include "semaphore.hpp"
#include <vector>
#include<queue>
class Boundedbuffer{
private:
    int n;
    Semaphore empty;
    Semaphore full;
    Mutex mutex;
    std::queue<std::string> q;
   
    
public:
    Boundedbuffer(int N):empty(N),full(0),mutex(){
      
       n=N;
    };
    ~Boundedbuffer(){
       
            };
  
    void Add(std::string item){
        empty.P();
        mutex.Lock();
        q.push(item);
        n++;
        mutex.Unlock();
        full.V();
     
    }
    
    
    std::string Remove(){
        full.P();
        mutex.Lock();
        if (q.empty()) {
            std::cout << n << std::endl;
            std::cout << "EMPTY" << std::endl;
        }
    
      std::string response =  q.front();
        q.pop();
        n--;
        mutex.Unlock();
        empty.V();
        return response;
        
    };
    
    
    
    
    
};

#endif
