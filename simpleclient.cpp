/* 
    File: simpleclient.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2013/01/31

    Simple client main program for MP3 in CSCE 313
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

#include <errno.h>
#include <unistd.h>

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
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

  std::cout << "CLIENT STARTED:" << std::endl;

  std::cout << "Establishing control channel... " << std::flush;
  RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
  std::cout << "done." << std::endl;;

  /* -- Start sending a sequence of requests */

  std::string reply1 = chan.send_request("hello");
  std::cout << "Reply to request 'hello' is '" << reply1 << "'" << std::endl;

  std::string reply2 = chan.send_request("data Joe Smith");
  std::cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << std::endl;

  std::string reply3 = chan.send_request("data Jane Smith");
  std::cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << std::endl;

  std::string reply5 = chan.send_request("newthread");
  std::cout << "Reply to request 'newthread' is " << reply5 << "'" << std::endl;
  RequestChannel chan2(reply5, RequestChannel::CLIENT_SIDE);

  std::string reply6 = chan2.send_request("data John Doe");
  std::cout << "Reply to request 'data John Doe' is '" << reply6 << "'" << std::endl;

  std::string reply7 = chan2.send_request("quit");
  std::cout << "Reply to request 'quit' is '" << reply7 << "'" << std::endl;

  std::string reply4 = chan.send_request("quit");
  std::cout << "Reply to request 'quit' is '" << reply4 << "'" << std::endl;

  usleep(1000000);
}
