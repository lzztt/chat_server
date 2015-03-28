/* 
 * File:   WebSocketServerApp.hpp
 * Author: ikki
 *
 * Created on February 20, 2015, 8:50 PM
 */

#ifndef WEBSOCKETSERVERAPP_HPP
#define	WEBSOCKETSERVERAPP_HPP

#include <sys/eventfd.h>
#include <vector>
#include <string>

#include "ClientSocketHandler.hpp"

class WebSocketServerApp
{
    friend class ClientSocketHandler;
public:

    WebSocketServerApp();

    WebSocketServerApp(const WebSocketServerApp& other) = delete;
    WebSocketServerApp& operator=(const WebSocketServerApp& other) = delete;

    WebSocketServerApp(WebSocketServerApp&& other) = delete;
    WebSocketServerApp& operator=(WebSocketServerApp&& other) = delete;

    ~WebSocketServerApp() = default;

    // event handler
    virtual void onOpen(int clientID) = 0;
    virtual void onClose(int clientID) = 0;
    virtual void onMessage(std::string msg, int clientID) = 0;
    virtual void onMessage(std::vector<unsigned char> msg, int clientID) = 0;

    // copy message
    virtual void send(std::string msg, int clientID) final;
    virtual void send(std::vector<unsigned char> msg, int clientID) final;
    virtual void send(std::string msg, std::vector<int> clientIDs) final;
    virtual void send(std::vector<unsigned char> msg, std::vector<int> clientIDs) final;

    // move message
    virtual void send(std::string&& msg, int clientID) final;
    virtual void send(std::vector<unsigned char>&& msg, int clientID) final;
    virtual void send(std::string&& msg, std::vector<int> clientIDs) final;
    virtual void send(std::vector<unsigned char>&& msg, std::vector<int> clientIDs) final;

    // close client
    virtual void close(int clientID) final;

    // start server
    virtual void run() final;

private:
    EventLoop myEventLoop;
    ClientSocketHandler myClientHandler;
    int mySendEventFd;
    std::vector<int> clientIDs;
    std::vector<int> messageIDs;
    std::deque<SocketOutStream::Data> messages;    
};

#endif	/* WEBSOCKETSERVERAPP_HPP */

