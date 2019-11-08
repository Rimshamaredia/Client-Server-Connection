
#include <sys/select.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include<map>
#include<iomanip>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>
#include "Boundedbuffer.hpp"
#include<vector>
#include "reqchannel.hpp"


using namespace std;



struct worker_thread_args{
    Boundedbuffer* wpc;
    std::map<string,Boundedbuffer*> mp;

    int size;
    std::vector<RequestChannel*> chan;

};

struct RTArgs{
    Boundedbuffer* wpc;
    int NReq;
    string patient_name;
};

struct STATArgs{
    Boundedbuffer* stat;
    string name;
    int n;


};

class Histogram{
private:
    map<string, int>mp;
    vector<string> name;
   int hist[3][10];
public:
    Histogram(){


            mp["Joe Smith"] = 0;
            name.push_back("Joe Smith");
            mp["Jane Smith"] = 1;
            name.push_back("Jane Smith");
            mp["John Smith"] = 2;
            name.push_back("John Doe");


    };
    void Add(string request,string response){
        int index = mp[request];
        hist[index][stoi(response)/10]++;
    };

    void print(){
        int temp[3];
        for(int i = 0;i<3;i++){
            cout<<setw(10)<<right<< name[i];
        }
        cout<<endl;
        for(int i = 0;i<10;i++){
            for(int j = 0;j<3;j++){
                cout<<setw(10)<<right<<hist[j][i];
                temp[j]+= hist[j][i];
            }
            cout<<endl;
        }


        for(int i  = 0;i<3;i++){
            cout<<setw(10)<<right<<temp[i];
            }
        cout<<endl;
    };
};
 Histogram hist;

void* worker_thread_Event(void* addr){

     worker_thread_args* wargs = (worker_thread_args*)addr;
    fd_set read_set;
    string request[wargs->size];

    for(int i  = 0;i<wargs->size;i++){
        request[i] = wargs->wpc->Remove();
        wargs->chan[i]->cwrite(request[i]);

    }
    while(true){
        FD_ZERO(&read_set);
        for(int i = 0;i<wargs->size;i++){
            FD_SET(wargs->chan[i]->read_fd(),&read_set);
        }

        //int sel = select(wargs->size,&read_set,NULL,NULL,NULL);
        int sel = select(getdtablesize()+1, &read_set,NULL,NULL,NULL);

            for(int i = 0;i<wargs->size;i++){
                if(FD_ISSET(wargs->chan[i]->read_fd(),&read_set)){
                    string response = wargs->chan[i]->cread();
                    cout<<"resuest: "<<request[i]<<endl;
                    cout<<"Response: "<<response<<endl;


                    // handle the quit
                    // remove the channel from the vector

                    // and continue
                    string name = request[i].substr(5);

                    wargs->mp[name]->Add(response);
                    request[i] = wargs->wpc->Remove();
                    // add a new quit if req[i] is quit
                    wargs->chan[i]->cwrite(request[i]);

                }
            }



    }
    for(int i  = 0;i< 100;i++){
        wargs->chan[i]->cwrite("quit");

    }
    return 0;
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


    STATArgs* st_args = (STATArgs*)addr;


    string response;
   for(int i = 0;i<st_args->n;i++){
        response = st_args->stat->Remove();
        if(response == "quit"){
            break;
        }else{

                   hist.Add(st_args->name, response);
        }


    }

    cout<<"-----------------------"<<endl;
    hist.print();

    return 0;
}



int main(int argc, char * argv[]) {
    int c = 0;
    int no_of_requests = 10;
    int buff_size = 50 ;
    int no_of_worker_threads = 100;
    Boundedbuffer* pcb = new Boundedbuffer(buff_size);
    cout<<"-n=# -w=# -b=#"<<endl;
   /* while((c= getopt(argc,argv,"n:w:b:"))!=-1){
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
    */
    /*pid_t pid ;//fork();
    pid = fork();
    if(pid ==0){
        cout<<"Starting server"<<endl;
        execl("./dataserver",NULL,NULL);

    }

    */
    RequestChannel* ctrlchan = new RequestChannel("control",RequestChannel::CLIENT_SIDE);






    vector<string> patientn = vector<string>(3);
    patientn.at(0)="Joe Smith";
    patientn.at(1)="Jane Smith";
    patientn.at(2)="John Doe";



    RTArgs rt[3];
    pthread_t *p[3];
cout<<"Creating Requests Threads"<<endl;
    for(int i = 0;i<3;i++){
        rt[i].patient_name = patientn[i];
        rt[i].NReq = no_of_requests;
        rt[i].wpc = pcb;
        p[i] = new pthread_t;
        pthread_create(p[i], NULL,req_thread, (void*)&rt[i]);
    }


    //stat threads
    pthread_t* stat_thread[3];

    vector<Boundedbuffer*> buffer;

    vector<STATArgs> st = vector<STATArgs>(3);

cout<<"Creating Statistics Thread"<<endl;

    for(int i = 0;i<3;i++){

        Boundedbuffer* b = new Boundedbuffer(10);
        buffer.push_back(b);
        st[i].stat = b;
        st[i].n = no_of_requests;
        st[i].name = patientn[i];


        stat_thread[i] = new pthread_t;
        pthread_create(stat_thread[i], NULL,STAT_thread, (void*)&st[i]);


    }


    pthread_t* worker_thread;

    worker_thread_args wt;
    wt.size = no_of_worker_threads;

    cout<<"Creating worker threads"<<endl;
    //channels



    wt.chan = std::vector<RequestChannel*>(no_of_worker_threads);
    wt.wpc = pcb;
    for(int i = 0;i<no_of_worker_threads;i++){

        string reply = ctrlchan->send_request("newthread");
        RequestChannel* newchan = new RequestChannel(reply,RequestChannel::CLIENT_SIDE);
        wt.chan[i] = newchan;

    }
    //names
    for(int j = 0;j<3;j++){
        wt.mp[patientn[j]] = buffer[j];
    }

    worker_thread = new pthread_t;

    pthread_create(worker_thread,NULL,worker_thread_Event,(void*)&wt);

    for(int i = 0;i<3;i++){
        pthread_join(*p[i],NULL);
    }
      //sending quit signal to all channels
    for(int i  = 0;i<100;i++){
        pcb->Add("quit");

    }

       //joining worker threads
        pthread_join(*worker_thread, NULL);
    //sending quit signal to stat buffers
    for(int i = 0;i<3;i++){
        buffer[i]->Add("quit");
    }
     //joining stat buffers
        for(int i = 0;i<3;i++){
            pthread_join(*stat_thread[i],NULL);

        }

        ctrlchan->cwrite("quit");
        delete ctrlchan;
        delete pcb;






        usleep(10000);

    }



