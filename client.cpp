
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

class histogram{
private:
    vector<int>hist = vector<int>(100);
public:
    
        void add(int val){
            hist[val]++;
        }
        void print(){
            
            for(int i = 0;i<hist.size();i++){
                
                cout<<i<<":"<<hist[i]<<" "<<endl;

            }
            cout<<endl;
        }
    
};

 

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
        
        string var = wargs->wpc->Remove();
        //cout<<"Value of var"<<var<<endl;
        if(var == "done"){
            wargs->chan->send_request("quit");
        }else{

        
       
        string reply = wargs->chan->send_request(var);
       // cout<<"Reply is "<<reply<<endl;
        string name = var.substr(5);
        
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
   
 vector<int> p_hist(100);
    STATArgs* st_args = (STATArgs*)addr;
    int i = 0;
    int j = 0;
    int k = 0;
    histogram hist;
    
   for(int i = 0;i<st_args->nreq;i++){
       string response = st_args->stat->Remove();
       int val = atoi(response.c_str());
       hist.add(val);
   }
 
        
   
   
   /*while(true){
        string response = st_args->stat->Remove();
      //cout<<"Response: "<<response<<endl;
      //cout<<"-----------------------------"<<endl;
      if(response == "done"){
          break;
      }else{
          int val =  atoi(response.c_str());
          //cout<<"Val "<<val<<endl;
         hist.add(val);
      }
    }
     */
        // utkarsh check if your removed item is a done. If done break else atoi and other things
   // cout<<"Histogram for "<<response<<endl;
  // cout<<"--------------------------------------------------------"<<endl;
   
    hist.print();
    cout<<endl;
    
    // utkarsh print hist before quiting
  
    return 0;
}



int main(int argc, char * argv[]) {
    int c = 0;
    int no_of_requests = 5;
    int buff_size = 50;
    int no_of_worker_threads = 3;
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
   
  

    int num_req_per_th = 10;
   
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
    
    r1.NReq = num_req_per_th;
    r2.NReq =num_req_per_th;
    r3.NReq =num_req_per_th;
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
        
          Boundedbuffer* pcb = new Boundedbuffer(10);
          buffer.push_back(pcb);
          st[i].stat = pcb;
        
          st[i].name = patientn[i];
          st[i].nreq = 10;
         cout<<"Here---------------------------"<<endl;
         // utkarsh corrected
          stat_thread[i] = new pthread_t;
          pthread_create(stat_thread[i], NULL,STAT_thread, (void*)&st[i]);
          
         
      }
  
    pthread_t* worker_threads[no_of_worker_threads];
    
    worker_thread_args wt[no_of_worker_threads];
    
    //RequestChannel* workerchan[3];
    
    for(int i = 0;i<no_of_worker_threads;i++){
        // utkarsh use the same wpc above
        wt[i].wpc = pcb;
        
        string reply = ctrlchan->send_request("newthread");
        //std::cout << "Reply is..." << reply << std::endl;
       wt[i].chan = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);
        for(int j = 0;j<3;j++){
            wt[i].mp[patientn[j]] = buffer[j];
        }
        // utkarsh corrected
        worker_threads[i] = new pthread_t;
        pthread_create(worker_threads[i],NULL,worker_thread,(void*)&wt[i]);
        
    }
   
      std::cout << "Escaped for loop" << std::endl;
   
    
  
    
    pthread_join(*p1, NULL);

    pthread_join(*p2, NULL);
    pthread_join(*p3, NULL);
    pcb->Add("done");
    cout<<"Killing worker thread"<<endl;
    // utkarsh to kill, send a done message and handle done messages in the worker thread

    for(int i = 1;i<no_of_worker_threads;i++){
        pthread_join(*worker_threads[i], NULL);
        wt[i].wpc->Add("done");

    }
    pcb->Add("done");
    
    // utkarsh kill stats thread by sending a done signal to all the stats buffers.

    for(int i = 0;i<3;i++){
        pthread_join(*stat_thread[i],NULL);
       
    }
    for(int i = 0;i<3;i++){
        st[i].stat->Add("done");
    }
   pcb->Add("done");
    
  for(int i = 0;i<no_of_worker_threads;i++){
              string reply = wt[i].chan->send_request("quit");
      delete wt[i].chan;
      
          }
        
         // delete wt;
       
    
    ctrlchan->send_request("quit");
    for(int i = 0;i<buffer.size();i++){
        delete buffer[i];
    }
    
    delete ctrlchan;
    
   
    
       usleep(10000);
 
}

