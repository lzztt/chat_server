/* 
 * File:   Socket.hpp
 * Author: ikki
 *
 * Created on February 18, 2015, 11:59 PM
 */

#ifndef SOCKET_HPP
#define	SOCKET_HPP

#include <string>

#include "Event.hpp"
#include "SocketDataHandler.hpp"

class Socket
{
public:
    explicit Socket(const int port, SocketDataHandler&& handler);
    explicit Socket(std::string& unixSocketFile, SocketDataHandler&& handler);

    Socket(const Socket& other) = delete;
    Socket& operator=(const Socket& other) = delete;

    Socket(Socket&& other) = delete;
    Socket& operator=(Socket&& other) = delete;

    ~Socket();
    
protected:
    void onConnect(const Event& ev);
    
private:
    int socket;
    SocketDataHandler handler;
};

#endif	/* SOCKET_HPP */

