/* 
 * File:   Server.cpp
 * Author: ikki
 * 
 * Created on February 20, 2015, 8:50 PM
 */

#include <sys/epoll.h>

#include "WebSocketServerApp.hpp"
#include "ServerSocket.hpp"
#include "EventLoop.hpp"

void WebSocketServerApp::run( )
{
    ServerSocket s( 8080, &myEventLoop, &myClientHandler );
    myEventLoop.registerEvent( Event( mySendEventFd, EPOLLIN & EPOLLOUT, [](const Event & ev)
    {
    } ) );
    
    myEventLoop.run( );
}

void WebSocketServerApp::close( int clientID )
{
    myClientHandler.remove( clientID );
}

// copy message

void WebSocketServerApp::send( std::string msg, int clientID )
{
}

void WebSocketServerApp::send( std::vector<char> msg, int clientID )
{
}

void WebSocketServerApp::send( std::string msg, std::vector<int> clientIDs )
{
}

void WebSocketServerApp::send( std::vector<char> msg, std::vector<int> clientIDs )
{
}

// move message

void WebSocketServerApp::send( std::string&& msg, int clientID )
{
}

void WebSocketServerApp::send( std::vector<char>&& msg, int clientID )
{
}

void WebSocketServerApp::send( std::string&& msg, std::vector<int> clientIDs )
{
}

void WebSocketServerApp::send( std::vector<char>&& msg, std::vector<int> clientIDs )
{
}