/* 
    File: requestchannel.cpp

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

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
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>

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
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* PRIVATE METHODS FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

char * RequestChannel::pipe_name(const Mode _mode) {
  std::string pname = "fifo_" + this->my_name;

  switch(_mode) {
  case Mode::READ: 
    pname += this->my_side == Side::CLIENT ? "1" : "2";
    break;
  case Mode::WRITE:
    pname += this->my_side == Side::CLIENT ? "2" : "1";
    break;
  }

  std::cout << "rqname = " << pname << std::endl;
  char * result = new char[pname.size()+1];
  strncpy(result, pname.c_str(), pname.size()+1);
  return result;
}

void RequestChannel::open_write_pipe(char * _pipe_name) {

  std::cout << "mkfifo write pipe " << _pipe_name << std::endl;

  if (mkfifo(_pipe_name, 0600) < 0) {
    if (errno != EEXIST) {
      perror("Error creating pipe for writing; exit program");
      exit(1);
    }
  }

  std::cout << "open write pipe" << std::endl;

  wfd = open(_pipe_name, O_WRONLY);
  if (wfd < 0) {
    perror("Error opening pipe for writing; exit program");
    exit(1);
  }

  std::cout << "done opening write pipe" << std::endl;

}

void RequestChannel::open_read_pipe(char * _pipe_name) {

  std::cout << "mkfifo read pipe " << _pipe_name << std::endl;

  if (mkfifo(_pipe_name, 0600) < 0) {
    if (errno != EEXIST) {
      perror("Error creating pipe for writing; exit program");
      exit(1);
    }
  }

  std::cout << "open read pipe" << std::endl;

  rfd = open(_pipe_name, O_RDONLY);
  if (rfd < 0) {
    perror("Error opening pipe for reading; exit program");
    exit(1);
  }

  std::cout << "done opening read pipe" << std::endl;
  
}

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

RequestChannel::RequestChannel(const std::string _name, const Side _side)
  : my_name(_name), my_side(_side) {

  // Here we need to be careful how we open the named pipe, because the
  // open call blocks until the other end has opened as well.
  // If we are not careful, the calls deadlock.
  if (_side == Side::SERVER) {
    open_write_pipe(pipe_name(Mode::WRITE));
    open_read_pipe(pipe_name(Mode::READ));
  } else {
    open_read_pipe(pipe_name(Mode::READ));
    open_write_pipe(pipe_name(Mode::WRITE));
  }
  
}

RequestChannel::~RequestChannel() {
  std::cout << "closing request channel '" << my_name << "'..." << std::endl;

  close(wfd);
  close(rfd);
  
  if (my_side == Side::SERVER) {
    
    std::cout << "closing IPC mechanisms on server side for channel '" << this->my_name << "'..." << std::endl;

    /* Destruct the underlying IPC mechanisms. */
    if (remove(pipe_name(Mode::READ)) != 0) {
      perror(std::string("Request Channel (" + this->my_name + ") : Error deleting pipe for reading").c_str());
    }
      
    if (remove(pipe_name(Mode::WRITE)) != 0) {
      perror(std::string("Request Channel (" + this->my_name + ") : Error deleting pipe for writing").c_str());
    }

    std::cout << "done closing IPC mechanisms on server side for channel '" << this->my_name << "'" << std::endl;
    
  }

  std::cout << "done closing request channel '" << my_name << "'" << std::endl;
  
}

/*--------------------------------------------------------------------------*/
/* READ/WRITE FROM/TO REQUEST CHANNELS  */
/*--------------------------------------------------------------------------*/

const int MAX_MESSAGE = 255;

std::string RequestChannel::send_request(std::string _request) {
  cwrite(_request);
  std::string s = cread();
  return s;
}

std::string RequestChannel::cread() {

  char buf[MAX_MESSAGE];

  if (read(rfd, buf, MAX_MESSAGE) < 0) {
    perror(std::string("Request Channel (" + this->my_name + "): Error reading from pipe!").c_str());
  }
  
  std::string s = buf;

  std::cout << "Request Channel (" << this->my_name << ") reads [" << buf << "]" << std::endl;
  
  return s;

}

void RequestChannel::cwrite(std::string _msg) {

  if (_msg.length() >= MAX_MESSAGE) {
    std::cerr << "Message too long for Channel!" << std::endl;
  }

  std::cout << "Request Channel (" << this->my_name << ") writing [" << _msg << "]";

  const char * s = _msg.c_str(); // NOTE: c_str() is NOT thread safe!!

  if (write(wfd, s, strlen(s)+1) < 0) {
    perror(std::string("Request Channel (" + this->my_name + ") : Error writing to pipe!").c_str());
  }

  std::cout << "(" << this->my_name << ") done writing." << std::endl;
  
}

/*--------------------------------------------------------------------------*/
/* ACCESS THE NAME OF REQUEST CHANNEL  */
/*--------------------------------------------------------------------------*/

std::string RequestChannel::name() {
  return my_name;
}

/*--------------------------------------------------------------------------*/
/* ACCESS FILE DESCRIPTORS OF REQUEST CHANNEL  */
/*--------------------------------------------------------------------------*/

int RequestChannel::read_fd() {
  return rfd;
}

int RequestChannel::write_fd() {
  return wfd;
}



