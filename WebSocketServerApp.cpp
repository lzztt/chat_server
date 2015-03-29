/* 
 * File:   Server.cpp
 * Author: ikki
 * 
 * Created on February 20, 2015, 8:50 PM
 */

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <cstring>

#include "WebSocketServerApp.hpp"
#include "ServerSocket.hpp"
#include "EventLoop.hpp"
#include "Log.hpp"

WebSocketServerApp::WebSocketServerApp( ) :
myClientHandler( ClientSocketHandler( this ) ),
myMessageQueueEventFd( ::eventfd( 0, EFD_NONBLOCK ) )
{

}

void WebSocketServerApp::run( )
{
    ServerSocket s( 8080, &myEventLoop, &myClientHandler );

    myEventLoop.registerEvent( Event( myMessageQueueEventFd, EPOLLIN, [this](const Event & ev)
    {
        this->myMessageQueueEventHandler( ev );
    } ) );

    myEventLoop.run( );
}

void WebSocketServerApp::myMessageQueueEventHandler( const Event & ev )
{
    // lock for out
    myMessageQueueMutex.lock( );

    uint64_t count = 0;
    ssize_t s = ::read( ev.getFd( ), &count, sizeof (count) );
    if ( s != sizeof (count) )
    {
        LOG_ERROR << "failed to read message queue event: " << std::strerror( errno );
    }

    if ( count != myMessageQueue.size( ) )
    {
        LOG_ERROR << "message queue size error: expecting " << count << ", actual has " << myMessageQueue.size( ) << " messages";
    }

    LOG_INFO << "flushing message queue";

    int i = 0, n = 0;
    size_t headerLength;
    unsigned char header[10];
    while ( !myMessageQueue.empty( ) )
    {
        Message& msg = myMessageQueue.front( );

        // build response header
        // text frame 0x81, binary frame 0x82
        header[0] = (msg.data.getType( ) == SocketOutStream::Data::Type::TEXT ? 0x81 : 0x82);
        size_t dataLength = msg.data.size( );
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
            LOG_ERROR << "message data size if too big: " << dataLength;
        }

        // copy data to send out
        for ( i = 1, n = msg.clients.size( ); i < n; ++i )
        {
            myClientHandler.streams[msg.clients[i]].out.add( std::vector<unsigned char>(header, header + headerLength) );
            myClientHandler.streams[msg.clients[i]].out.add( SocketOutStream::Data( msg.data ) );
            // try send now
            myClientHandler.onSend( Event( msg.clients[i], 0, Event::dummyEventHandler ) );
        }
        // move data to first client
        myClientHandler.streams[msg.clients[0]].out.add( std::vector<unsigned char>(header, header + headerLength) );
        myClientHandler.streams[msg.clients[0]].out.add( std::move( msg.data ) );
        // try send now
        myClientHandler.onSend( Event( msg.clients[0], 0, Event::dummyEventHandler ) );
        
        // remove front
        myMessageQueue.pop_front( );
    }

    myMessageQueueMutex.unlock( );
}

void WebSocketServerApp::close( int clientID )
{
    myClientHandler.close( clientID );
}

// copy message

void WebSocketServerApp::send( std::string msg, int clientID )
{
    myMessageQueueMutex.lock( );
    LOG_INFO << "sending-message appended to queue for client " << clientID;
    myMessageQueue.push_back( Message{SocketOutStream::Data( std::move( msg ) ), std::vector<int>(1, clientID)} );
    uint64_t count = 1;
    ssize_t s = ::write( myMessageQueueEventFd, &count, sizeof (count) );
    if ( s != sizeof (count) )
    {
        LOG_ERROR << "failed to write message queue event: " << std::strerror( errno );
    }
    myMessageQueueMutex.unlock( );
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