#ifndef Boundedbuffer_hpp
#define Boundedbuffer_hpp
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
    Semaphore mutex;
    std::queue<std::string> q;
   
    
public:
    Boundedbuffer(int N):empty(N),full(0),mutex(1){
      
       n=N;
    };
    ~Boundedbuffer(){
       
            };
  
    void Add(std::string item){
        empty.P();
        mutex.P();
        q.push(item);
        n++;
        mutex.V();
        full.V();
     
    }
    
    
    std::string Remove(){
        full.P();
        mutex.P();
        if (q.empty()) {
            std::cout << n << std::endl;
            std::cout << "EMPTY" << std::endl;
        }
    
      std::string response =  q.front();
        q.pop();
        n--;
        mutex.V();
        empty.V();
        return response;
        
    };
    
    
    
    
    
};

#endif
