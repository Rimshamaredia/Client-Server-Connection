/* 
    File: dataserver.cpp

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 

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

#include "reqchannel.hpp"

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

static int nthreads = 0;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

void handle_process_loop(RequestChannel & _channel);

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
  return int2string(rand() % 100);
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THREAD FUNCTIONS */
/*--------------------------------------------------------------------------*/

void * handle_data_requests(void * args) {

  RequestChannel * data_channel =  (RequestChannel*)args;

  // -- Handle client requests on this channel. 
  
  handle_process_loop(*data_channel);

  // -- Client has quit. We remove channel.
 
  delete data_channel;

  return nullptr; // keep compiler happy
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/

void process_hello(RequestChannel & _channel, const std::string & _request) {
  _channel.cwrite("hello to you too");
}

void process_data(RequestChannel & _channel, const std::string &  _request) {
  std::string data = generate_data();
  _channel.cwrite(data);
}

void process_newthread(RequestChannel & _channel, const std::string & _request) {
  int error;
  nthreads ++;

  // -- Name new data channel

  std::string new_channel_name = "data" + int2string(nthreads) + "_";
 
  // -- Pass new channel name back to client

  _channel.cwrite(new_channel_name);

  // -- Construct new data channel (pointer to be passed to thread function)
  
  RequestChannel * data_channel = new RequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);

  // -- Create new thread to handle request channel

  pthread_t thread_id;
  //  std::cout << "starting new thread " << nthreads << std::endl;
  if ((error = pthread_create(& thread_id, nullptr, handle_data_requests, data_channel))) {
    fprintf(stderr, "p_create failed: %s\n", strerror(error));
  }  

}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/

void process_request(RequestChannel & _channel, const std::string & _request) {

  if (_request.compare(0, 5, "hello") == 0) {
    process_hello(_channel, _request);
  }
  else if (_request.compare(0, 4, "data") == 0) {
    process_data(_channel, _request);
  }
  else if (_request.compare(0, 9, "newthread") == 0) {
    process_newthread(_channel, _request);
  }
  else {
    _channel.cwrite("unknown request");
  }

}

void handle_process_loop(RequestChannel & _channel) {

  for(;;) {

    std::cout << "Reading next request from channel (" << _channel.name()
	      << ") ..." << std::flush;
    std::string request = _channel.cread();
    std::cout << " done (" << _channel.name() << ")." << std::endl;
    std::cout << "New request is " << request << std::endl;

    if (request.compare("quit") == 0) {
      _channel.cwrite("bye");
      usleep(10000);          // give the other end a bit of time.
      break;                  // break out of the loop;
    }

    process_request(_channel, request);
  }
  
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

  //  std::cout << "Establishing control channel... " << std::flush;
  RequestChannel control_channel("control", RequestChannel::SERVER_SIDE);
  //  std::cout << "done.\n" << std::endl;

  handle_process_loop(control_channel);

}

