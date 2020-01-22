#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
#include "Client.h"

#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 

// The amount to read in before we send a packet
const unsigned int stdin_bufsize = 50;
const unsigned int socket_bufsize = 100;

class TCPClient : public Client
{
public:
   TCPClient();
   ~TCPClient();

   virtual void connectTo(const char *ip_addr, unsigned short port);
   virtual void handleConnection();

   virtual void closeConn();

private:

   int clientSocketFD;
   struct sockaddr_in clientAddress;

   void sendMessage(std::string msg);

};


#endif
