#include "TCPServer.h"
#include <iostream>

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 

#include<arpa/inet.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 

#include <algorithm>

TCPServer::TCPServer() {
    std::cout << "Creating a server...\n";
}


TCPServer::~TCPServer() {

}

/**********************************************************************************************
 * bindSvr - Creates a network socket and sets it nonblocking so we can loop through looking for
 *           data. Then binds it to the ip address and port
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPServer::bindSvr(const char *ip_addr, short unsigned int port) {
    
    //Create network socket
    std::cout << "Attempting to create network socket\n";
    serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocketFD == 0)
    {
        std::cout << "Error when creating servers socket!\n";
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }
    else
    {
        std::cout << "Socket created.\n";
    }

 

    //Set socket to nonblocking
    std::cout << "Setting socket to nonblocking\n";
    int opt = 1;
    if(setsockopt(serverSocketFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        std::cout << "Error when setting socket to unblocking!\n";
    };

    //Bind socket to IP address and port
    std::cout << "Attempting to bind network socket\n";

     //Set up our socket address struct
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(ip_addr);

    if (bind(serverSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress))<0) 
    { 
        std::cout << "Error when attempting to bind network socket!\n";
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    else
    {
        std::cout << "Socket bound.\n";
    }
    if(listen(serverSocketFD, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Waiting for connections...\n";
   
}

/**********************************************************************************************
 * listenSvr - Performs a loop to look for connections and create TCPConn objects to handle
 *             them. Also loops through the list of connections and handles data received and
 *             sending of data. 
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPServer::listenSvr() {

    fd_set readFD;

    while(true)
    {
        FD_ZERO(&readFD);
        FD_SET(serverSocketFD, &readFD);
        int maxSocket = serverSocketFD;

        //Add child sockets
        for(auto i : clientSockets)
        {
            int socket = i;
            //If socket is valid, add it to set
            FD_SET(socket, &readFD);
            //Update max socket number, if necessary
            if(socket > maxSocket) { maxSocket = socket; }
        }


        //Wait for activity
        int activity = select(maxSocket + 1, &readFD, NULL, NULL, NULL);
        if((activity < 0) && (errno != EINTR))
        {
            std::cout << "SELECT ERROR\n";
        }

        //If something happens, we probably have an incoming connection
        if(FD_ISSET(serverSocketFD, &readFD))
        {
            int addrlen = sizeof(serverAddress);
            int newSocket = accept(serverSocketFD, (struct sockaddr *)&serverAddress, (socklen_t *)&addrlen);
            if(newSocket > 0)
            {
                std::cout << "New connection: socketFD=" << newSocket << "\n";

                clientSockets.push_back(newSocket);
                sendMessage(newSocket, "Welcome!\n");
                printMenu(newSocket);
                
            }
        
        }
        //Check the client sockets
        for(auto it = clientSockets.begin(); it != clientSockets.end(); it++)
        {
            auto client = (*it);
            if(FD_ISSET(client, &readFD))
            {
                // char readIn[1024];
                // if(read(client, readIn, 1023) > 0)
                // {
                //     // //Print the message
                //     std::cout << readIn << '\n';
                //     // //Send the message back out
                //     // sendMessage(client, readIn);
                // }
                // //std::cout << input << "\n";
                // else
                // {
                //     std::cout << "ClientFD= " << client << "has disconnected.\n";
                //     //Close the socket
                //     close(client);
                //     clientSockets.erase(it);
                // }
                // char buffer[1024];
                // ssize_t read = recv(client, buffer, strlen(buffer), MSG_DONTWAIT);
                char clientMessage[1024];
                clientMessage[0] = '\0';
                int readval = read(client, clientMessage, 1024);
                if(readval > 0)
                {
                    clientMessage[readval] = '\0';
                    std::cout << clientMessage << "\n";
                    performServerAction(client, clientMessage);
                }
                else
                {
                    std::cout << "Error reading from client socket " << client << ". Terminating connection.\n";
                    close(client);
                    clientSockets.erase(it);
                }
                

            }
            
        }
        

        
    }


}

/**********************************************************************************************
 * shutdown - Cleanly closes the socket FD.
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPServer::shutdown() {
    std::cout << "Shutting down server...\n";
    close(serverSocketFD);
}


void TCPServer::sendMessage(int clientFD, std::string msg)
{
    //Send over socket
    send(clientFD, msg.c_str(), msg.length(), 0);

}


void TCPServer::printMenu(int clientFD)
{
    sendMessage(clientFD, "\n----MENU----\n");
    sendMessage(clientFD, "Please select one of the following options:\n");
    sendMessage(clientFD, "hello: say hello!\n");
    sendMessage(clientFD, "1-5: Display Interesting Facts\n");
    sendMessage(clientFD, "password: Display password\n");
    sendMessage(clientFD, "exit: Disconnect\n");
    sendMessage(clientFD, "menu: Display this menu\n");
    sendMessage(clientFD, "\n");

}

void TCPServer::interestingFacts(int clientFD, int fact)
{
    switch(fact)
    {
        case 1:
            sendMessage(clientFD, "Interesting Fact 1\n");
            break;
        case 2:
            sendMessage(clientFD, "InterestingFact 2\n");
            break;
        case 3:
            sendMessage(clientFD, "InterestingFact 3\n");
            break;
        case 4:
            sendMessage(clientFD, "InterestingFact 4\n");
            break;
        case 5:
            sendMessage(clientFD, "Interesting Fact 5\n");
            break;
        default:
            sendMessage(clientFD, "Default Fact\n");
    }

}

void TCPServer::displayPassword(int clientFD)
{
    sendMessage(clientFD, "This feature is not yet implemented\n");
}

void TCPServer::sayHello(int clientFD)
{
    sendMessage(clientFD, "Hello to you too!\n");
}

void TCPServer::disconnectClient(int clientFD)
{
    sendMessage(clientFD, "See you later!\n");\
    std::cout << "ClientFD= " << clientFD << "has disconnected.\n";


    //Close the socket
    close(clientFD);
    //Find the iterator for the client
    auto it = std::find(clientSockets.begin(), clientSockets.end(), clientFD);
    clientSockets.erase(it);
}


void TCPServer::performServerAction(int clientFD, std::string userString)
{
    if(userString == "hello")
    {
        sayHello(clientFD);
    }
    else if (userString == "password")
    {
        displayPassword(clientFD);
    }
    else if (userString == "1")
    {
        interestingFacts(clientFD, 1);
    }
    else if (userString == "2")
    {
        interestingFacts(clientFD, 2);
    }
    else if (userString == "3")
    {
        interestingFacts(clientFD, 3);
    }
    else if (userString == "4")
    {
        interestingFacts(clientFD, 4);
    }
    else if (userString == "5")
    {
        interestingFacts(clientFD, 5);
    }
    else if (userString == "exit")
    {
        disconnectClient(clientFD);
    }
    else if(userString == "menu")
    {
        printMenu(clientFD);
    }
    else
    {
        sendMessage(clientFD, "I didn't understand that command. Please try again.\n");
    }
}