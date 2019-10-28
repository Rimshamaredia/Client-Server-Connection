
#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include<map>
#include<iomanip>
#include <errno.h>
#include <unistd.h>
#include "Boundedbuffer.hpp"
#include<vector>
#include "reqchannel.hpp"


using namespace std;



struct worker_thread_args{
    Boundedbuffer* wpc;
    std::map<string,Boundedbuffer*> mp;
    RequestChannel* chan;
  
};

struct RTArgs{
    Boundedbuffer* wpc;
    int NReq;
    string patient_name;
};

struct STATArgs{
    Boundedbuffer* stat;
    string name;
    
    int nreq;
};


void* worker_thread(void* addr){
    worker_thread_args* wargs = (worker_thread_args*)addr;
  
   
    while(true){
        // utkarsh check if var is done, quit if yes. else do regular stuff.
        
        string request = wargs->wpc->Remove();
      // cout<<"--------------------------"<<endl;
      // cout<<request<<endl;
        if(request == "done"){
            wargs->chan->send_request("quit");
            delete wargs->chan;
         
        }else{

        
       
        string reply = wargs->chan->send_request(request);
       // cout<<"Reply is "<<reply<<endl;
        string name = request.substr(5);
        
       wargs->mp[name]->Add(reply);
       
        }
    }
    
}


   

// request thread
void *req_thread(void* addr){
    RTArgs* args = (RTArgs*)addr;
  
    for(int i = 0;i<args->NReq;i++){
        args->wpc->Add("data " + args->patient_name);
    }
    return 0;
}


void* STAT_thread(void* addr){
   
vector<int>p_hist(100);
    STATArgs* st_args = (STATArgs*)addr;
    
    
    string response;
 for(int i = 0;i<st_args->nreq;i++){
        response = st_args->stat->Remove();
       int val = atoi(response.c_str());
      
      p_hist[val]++;
   }

  

    cout<<"histogram: "<<endl;
 for(int i = 0;i<p_hist.size();i++){
     cout<<i<<": "<<p_hist[i]<<endl;
 }
 cout<<endl;
    
       
   
    
 
  
    return 0;
}



int main(int argc, char * argv[]) {
    int c = 0;
    int no_of_requests;
    int buff_size ;
    int no_of_worker_threads;
    Boundedbuffer* pcb = new Boundedbuffer(buff_size);
    cout<<"-n=# -w=# -b=#"<<endl;
    while((c= getopt(argc,argv,"n:w:b:"))!=-1){
        switch(c){
            case 'n':
                no_of_requests = atoi(optarg);
                break;
            case 'w':
                no_of_worker_threads = atoi(optarg);
            case 'b':
                buff_size = atoi(optarg);
                break;
            case '?':
                abort();
            default:
                cout<<"Error";
                abort();
                
        }
    }
    
     pid_t pid ;//fork();
    pid = fork();
     if(pid ==0){
        cout<<"Starting server"<<endl;
        execl("./dataserver",NULL,NULL);
    
     }
    
    RequestChannel* ctrlchan = new RequestChannel("control",RequestChannel::CLIENT_SIDE);
    
    string reply1 = ctrlchan->send_request("hello");
    cout<<"Reply to request 'hello' is "<<reply1<<endl;
    
   
    
   
 
    vector<string> patientn = vector<string>(3);
    patientn.at(0)="Joe Smith";
    patientn.at(1)="Jane Smith";
    patientn.at(2)="John Smith";
   
  


   
    RTArgs r1;
    RTArgs r2;
    RTArgs r3;
    
    // utkarsh same bb needed
    r1.wpc = pcb;
    r2.wpc = pcb;
    r3.wpc = pcb;
    
    r1.patient_name = "Joe Smith";
    r2.patient_name = "Jane Smith";
    r3.patient_name = "John Smith";
    
    r1.NReq = no_of_requests;
    r2.NReq =no_of_requests;
    r3.NReq =no_of_requests;
    //request threads
    pthread_t* p1 = new pthread_t;
    pthread_t* p2 = new pthread_t;
    pthread_t* p3 = new pthread_t;
  
    pthread_create(p1,NULL,req_thread,(void*)&r1);
    
    pthread_create(p2,NULL,req_thread,(void*)&r2);
    
    pthread_create(p3,NULL,req_thread,(void*)&r3);
    
    
    
    //stat threads
    pthread_t* stat_thread[3];
   
    vector<Boundedbuffer*> buffer;
      
   vector<STATArgs> st = vector<STATArgs>(3);
  
  //STATArgs st[3];
    
      for(int i = 0;i<3;i++){
        
          Boundedbuffer* b = new Boundedbuffer(10);
           buffer.push_back(b);
          st[i].stat = b;
        
          st[i].name = patientn[i];
          st[i].nreq = 10;
         cout<<"Here---------------------------"<<endl;
         // utkarsh corrected
          stat_thread[i] = new pthread_t;
          pthread_create(stat_thread[i], NULL,STAT_thread, (void*)&st[i]);
          
         
      }
  
    pthread_t* worker_threads[no_of_worker_threads];
    
    worker_thread_args wt[no_of_worker_threads];
    
  
    
    for(int i = 0;i<no_of_worker_threads;i++){
       
        wt[i].wpc = pcb;
        
        string reply = ctrlchan->send_request("newthread");
      
       wt[i].chan = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);
        for(int j = 0;j<3;j++){
            wt[i].mp[patientn[j]] = buffer[j];
        }
       
        worker_threads[i] = new pthread_t;
        pthread_create(worker_threads[i],NULL,worker_thread,(void*)&wt[i]);
        
    }
   

   
    
  
    //join request threads
    pthread_join(*p1, NULL);

    pthread_join(*p2, NULL);
    pthread_join(*p3, NULL);
    pcb->Add("done");
 
   
    for(int i = 0;i<no_of_worker_threads;i++){
        pthread_join(*worker_threads[i], NULL);


    }
    pcb->Add("done");
    
  
    for(int i = 0;i<3;i++){
        pthread_join(*stat_thread[i],NULL);
       
    }
 
 
    
  for(int i = 0;i<no_of_worker_threads;i++){
              
      delete wt[i].chan;
      
      
      
          }
        
       
    
    ctrlchan->send_request("quit");
    delete ctrlchan;
    delete pcb;

  
    
    
   
    
       usleep(10000);
 
}

