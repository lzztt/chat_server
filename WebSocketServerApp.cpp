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
#include "Log.hpp"

WebSocketServerApp::WebSocketServerApp( ) :
myClientHandler( ClientSocketHandler( this ) ),
mySendEventFd( ::eventfd( 0, 0 ) )
{

}

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
    myClientHandler.close( clientID );
}

// copy message

void WebSocketServerApp::send( std::string msg, int clientID )
{
    // build response header
    size_t headerLength;
    unsigned char header[10];
    // text frame for now
    header[0] = 0x81;
    size_t dataLength = msg.size( );
    if ( dataLength < 0x7E )
    {
        headerLength = 2;
        header[1] = dataLength;
    }
    else if ( dataLength <= 0xFFFF )
    {
        headerLength = 4;
        header[1] = 0x7E;
        *((uint16_t*) (header + 2)) = htobe16( dataLength );
    }
    else if ( dataLength <= 0x7FFFFFFFFFFFFFFF )
    {
        headerLength = 10;
        header[1] = 0x7F;
        *((uint64_t*) (header + 2)) = htobe64( dataLength );
    }
    else
    {
        // too big, not handling
        LOG_ERROR << "response size if too big: " << dataLength;
    }

    myClientHandler.streams[clientID].out.add( std::vector<unsigned char>(header, header + headerLength) );
    myClientHandler.streams[clientID].out.add( std::move( msg ) );
}

void WebSocketServerApp::send( std::vector<unsigned char> msg, int clientID )
{
}

void WebSocketServerApp::send( std::string msg, std::vector<int> clientIDs )
{
}

void WebSocketServerApp::send( std::vector<unsigned char> msg, std::vector<int> clientIDs )
{
}

// move message

void WebSocketServerApp::send( std::string&& msg, int clientID )
{
}

void WebSocketServerApp::send( std::vector<unsigned char>&& msg, int clientID )
{
}

void WebSocketServerApp::send( std::string&& msg, std::vector<int> clientIDs )
{
}

void WebSocketServerApp::send( std::vector<unsigned char>&& msg, std::vector<int> clientIDs )
{
}