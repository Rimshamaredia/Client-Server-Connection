# makefile

# uncomment the version of the compiler that you are using
#clang++ is for Mac OS 
#C++ = clang++ -std=c++11
# g++ is for most Linux
C++ = g++ -std=c++11

.DEFAULT_GOAL :=all
clean:
	rm -f *.o dataserver client

all: dataserver client 

reqchannel.o: reqchannel.hpp reqchannel.cpp
	$(C++) -c -g reqchannel.cpp
mutex.o: mutex.hpp mutex.cpp
	$(C++) -c -g mutex.cpp 

semaphore.o: semaphore.hpp semaphore.cpp
	$(C++) -c -g semaphore.cpp 

BoundedBuffer.o: Boundedbuffer.hpp  semaphore.o
	$(C++) -c -g  semaphore.o

dataserver: dataserver.cpp reqchannel.o 
	$(C++) -g -o dataserver dataserver.cpp reqchannel.o -lpthread

client: client.cpp Boundedbuffer.o mutex.o semaphore.o reqchannel.o
	$(C++) -g -o client client.cpp Boundedbuffer.o mutex.o semaphore.o reqchannel.o -lpthread
