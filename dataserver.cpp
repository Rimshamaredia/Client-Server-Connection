/*
 File: dataserver.cpp
 
 Author: R. Bettati
 Department of Computer Science
 Texas A&M University
 Date  : 2016/07/14
 
 Dataserver main program for MP2 in CSCE 313
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>


#include "netreqchannel.hpp"
/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* VARIABLES */
/*--------------------------------------------------------------------------*/
const int MAX_MSG = 255;
/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

std::string int2string(int number) {
    std::stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- GENERATE THE DATA */
/*--------------------------------------------------------------------------*/

std::string generate_data() {
    // Generate the data to be returned to the client.
   usleep(1000 + (rand() % 5000));
  return int2string(rand() % 100);
}

std:: string server_rd(int * fd){
    char buff[MAX_MSG];
    read(*fd, buff, MAX_MSG);
    string s = buff;
    return s;
}
void server_write(int*fd, string m){
   
    if(write(*fd,m.c_str(),m.length()+1)<0){
        
        cerr<<"Error writing"<<endl;
    }
 
}


void process_hello(int* fd, const std::string & _request) {
    server_write(fd, "Hello");
    
}

void process_data(int *fd, const std::string &  _request) {
    
    std::string data = generate_data();

    server_write(fd, data);
    
    
    
}



/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/
/*void process_request(int* fd, const std::string & _request){
    
    if (_request.compare(0, 5, "hello") == 0) {
        process_hello(fd, _request);
    }
    else if (_request.compare(0, 4, "data") == 0) {
        process_data(fd, _request);
    }else{
        server_write(fd,"Unknown request");
    }
    return;
    
   
}
*/

void* handle_connection(void* arg){
  int *fd = (int*)arg;
  cout << "Start conn handle" << endl;
  string request = server_rd(fd);
  
   while(true){
        if (request.compare("quit") == 0) {
            cout<<"request:"<<request<<endl;
            server_write(fd,"bye");
             
            break;
        }
        else if (request.compare(0, 5, "hello") == 0) {
            process_hello(fd, request);
            
        }
        else if (request.compare(0, 4, "data") == 0) {
            process_data(fd, request);
            
        }else{
            server_write(fd,"Unknown request");
        }
        cout << "Reading new req" << endl;
        request = server_rd(fd);
    }
    close(*fd);
    cout<<"Connection finished"<<endl;
}



/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    
    
   unsigned short port_num = 8000;
    int c = 0;
    int backlog = 32;
    
    while((c= getopt(argc,argv,"n:b:"))!=-1){
        switch (c) {
            case 'n':
                port_num = atoi(optarg);
                break;
            case 'b':
                backlog = atoi(optarg);
                break;
            default:
                abort();
          
        }
    }
        cout<<"Server started on port: "<<port_num<<endl;
    NetworkRequestChannel server(port_num,handle_connection);
    
        server.~NetworkRequestChannel();
    
}


