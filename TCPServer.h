#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "Server.h"

#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  


#include <vector>
#include <string>

class TCPServer : public Server 
{
public:
   TCPServer();
   ~TCPServer();

   void bindSvr(const char *ip_addr, unsigned short port);
   void listenSvr();
   void shutdown();


   int MaxClients = 10;

private:

   void printMenu(int clientFD);
   void sendMessage(int clientFD, std::string msg);

   void interestingFacts(int clientFD, int fact);
   void displayPassword(int clientFD);
   void sayHello(int clientFD);
   void disconnectClient(int clientFD);

   void performServerAction(int clientFD, std::string action);


   int serverSocketFD;
   struct sockaddr_in serverAddress;
   std::vector<int> clientSockets;
 


};


#endif
