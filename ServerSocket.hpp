/* 
 * File:   ServerSocket.hpp
 * Author: ikki
 *
 * Created on February 18, 2015, 11:59 PM
 */

#ifndef SERVERSOCKET_HPP
#define	SERVERSOCKET_HPP

#include <string>

#include "Event.hpp"
#include "ClientSocketHandler.hpp"

class ServerSocket
{
public:
    explicit ServerSocket(const int port);
    explicit ServerSocket(std::string& unixSocketFile);

    ServerSocket(const ServerSocket& other) = delete;
    ServerSocket& operator=(const ServerSocket& other) = delete;

    ServerSocket(ServerSocket&& other) = delete;
    ServerSocket& operator=(ServerSocket&& other) = delete;

    ~ServerSocket();
    
protected:
    void onConnect(const Event& ev);
    
private:
    int socket;
    ClientSocketHandler* pClientHandler;
};

#endif	/* SERVERSOCKET_HPP */

