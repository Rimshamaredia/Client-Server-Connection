#include "netreqchannel.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <string.h>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>

struct sockaddr_in sin;
int NetworkRequestChannel::connectTCPClient(const char*host,const char* service){
//int NetworkRequestChannel::connectTCPClient(const char*host, unsigned short port){
    struct sockaddr_in sockIn;
    memset(&sockIn, 0, sizeof(sockIn));
    sockIn.sin_family = AF_INET;

//	if(struct servent * pse = getservbyport(port, "tcp")){
    if(struct servent * pse = getservbyname(service, "tcp")){
        sockIn.sin_port = pse->s_port;
    }else if ((sockIn.sin_port = htons((unsigned short)atoi(service))) == 0){
//    }else if ((sockIn.sin_port = htons(port)) == 0){
        cerr << "cant connect port"<<endl;
    }


    if(struct hostent * hn = gethostbyname(host)){
        memcpy(&sockIn.sin_addr, hn->h_addr, hn->h_length);
    }


    else if((sockIn.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE){
        cerr << "cant determine host " << host << endl;
    }

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0){
        cerr << "cant create socket\n";
    }


    if(connect(s, (struct sockaddr *)&sockIn, sizeof(sockIn)) < 0){
        cerr << "cant connect to " << host << ":" << service<<endl;
//        cerr << "cant connect to " << host << ":" << port<<endl;
    }
    else
    {
        cout<<"Sucess"<<endl;
    }


    //cout<<"Sucess"<<endl;
    return s;
}
NetworkRequestChannel::NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no){

    stringstream ss;
    ss<<_port_no;

    string port = ss.str();
    fd = connectTCPClient(_server_host_name.c_str(), port.c_str());
//    fd = connectTCPClient(_server_host_name.c_str(), _port_no);





}
int NetworkRequestChannel::connectTCPServer(const char* svc){
    int socket_num = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_num<0)
        cerr<<"Cannot create socket"<<endl;
    // struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    if(struct servent* pse = getservbyname(svc,"tcp")){
        sin.sin_port = pse->s_port;
    }

    else if ((sin.sin_port = htons((unsigned short)atoi(svc)))==0){
        cerr<<"Cannot get port"<<endl;
    }





    if(::bind(socket_num, (struct sockaddr*)&sin,sizeof(sin)) < 0){
        cerr<<"Cannot bind to port"<<endl;
    }

    listen(socket_num,100);

    return socket_num;

}


NetworkRequestChannel::NetworkRequestChannel(const unsigned short _port_no, void *(*connection_handler)(void*)){
    stringstream ss ;
    ss<<_port_no;
    string port = ss.str();

    int m_socket = connectTCPServer(port.c_str());




    int server_size = sizeof(sin);

    while(true){


        int *s_socket = new int;
        pthread_t *th = new pthread_t;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        *s_socket = accept(m_socket, (struct sockaddr*)&sin, (socklen_t*)&server_size);

        if(s_socket<(void*)0){
            delete s_socket;
            if(errno == EINTR) {
                continue;

            }
            else{
                cerr<<"Accept failed"<<endl;
            }
        }
        cout << "New connection created " << *s_socket << endl;
        pthread_create(th, &attr, connection_handler,(void*)s_socket);

    }

    cout<<"Server Connection Done"<<endl;
}


void NetworkRequestChannel:: cwrite(string _msg){
    if(_msg.length()>=MAX_MESSAGE){
        cerr<<"Messgae too long for the channel"<<endl;

    }
    const char* s = _msg.c_str();
    if(write(fd,s,strlen(s)+1)<0){
        cerr<<"Error writing to the pipe"<<endl;
    }


}

string NetworkRequestChannel::cread(){
    char buff[MAX_MESSAGE];
    if(read(fd,buff,MAX_MESSAGE)<0){
        cerr<<"Error reading"<<endl;
    }
    string s = buff;
    return s;
}


NetworkRequestChannel::~NetworkRequestChannel(){
    close(fd);
};



