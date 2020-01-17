#include "TCPClient.h"
#include <iostream>

#include <stdio.h> 
#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string>  



/**********************************************************************************************
 * TCPClient (constructor) - Creates a Stdin file descriptor to simplify handling of user input. 
 *
 **********************************************************************************************/

TCPClient::TCPClient() {
    //std::cout << "Creating a client with STDin file descripter!\n";
    
}

/**********************************************************************************************
 * TCPClient (destructor) - No cleanup right now
 *
 **********************************************************************************************/

TCPClient::~TCPClient() {

}

/**********************************************************************************************
 * connectTo - Opens a File Descriptor socket to the IP address and port given in the
 *             parameters using a TCP connection.
 *
 *    Throws: socket_error exception if failed. socket_error is a child class of runtime_error
 **********************************************************************************************/

void TCPClient::connectTo(const char *ip_addr, unsigned short port) {


    std::cout << "Attempting to connect to some IP address...\n";
    clientSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocketFD < 0)
    {
        std::cout << "Error creating client socket!\n";
        return;
    }

    clientAddress.sin_family = AF_INET;
    clientAddress.sin_port = htons(port);

    if(inet_pton(AF_INET, ip_addr, &clientAddress.sin_addr) <= 0)
    {
        std::cout << "IP address is invalid or unsupported.\n";
    }

    //Attempt to connect
    int connectionStatus = connect(clientSocketFD, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
    if(connectionStatus < 0)
    {
        std::cout << "Connection failed!";
        return;
    }

}

/**********************************************************************************************
 * handleConnection - Performs a loop that checks if the connection is still open, then 
 *                    looks for user input and sends it if available. Finally, looks for data
 *                    on the socket and sends it.
 * 
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPClient::handleConnection() {
   

   //Send a message
   //sendMessage("Knock knock, anyone home?\n");
  //While (user has not chosen to kill the connection)
  while(true)
  {
      //Print output from server
      char serverMessage[1024];
      serverMessage[0] = '\0';
      int readval = read(clientSocketFD, serverMessage, 1024);
      if(readval > 0)
      {
            serverMessage[readval] = '\0';
            std::cout << '\n' << serverMessage;

            //Get any user input
            std::string userInput;
        
            //std::cin >> userInput;
            std::cin.clear();
            std::getline(std::cin, userInput);

            //If input exists, send it
            //std::cout << "Sending message: " << userInput;
            sendMessage(userInput);
      }
      else //Connection is no longer open
      {
          std::cout << "Connection has been closed unexpectedly.\n";
          close(clientSocketFD);
          return; 
      }

  }
}

/**********************************************************************************************
 * closeConnection - Your comments here
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPClient::closeConn() {

    std::cout << "Closing connection...\n";
    close(clientSocketFD);
}

void TCPClient::sendMessage(std::string msg)
{
    //Send char array
    //send(clientSocketFD, msg.data(), msg.size(), 0);
    //send(clientSocketFD, msg.c_str(), msg.length(),0);
   //auto writestatus = write(clientSocketFD, msg.c_str(), msg.length());
   send(clientSocketFD, msg.c_str(), msg.length(), 0);

}




