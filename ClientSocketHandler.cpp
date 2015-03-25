/* 
 * File:   SocketDataHandler.cpp
 * Author: ikki
 * 
 * Created on February 20, 2015, 11:25 PM
 */

#include <sys/epoll.h>
#include <sys/socket.h>

#include "SocketDataHandler.hpp"
#include "EventLoop.hpp"
#include "Log.hpp"
#include "SocketInStream.hpp"
#include "SocketOutStream.hpp"
#include "MessageHandler.hpp"
#include "HandshakeHandler.hpp"
#include "DataFrameHandler.hpp"

class SocketDataHandler::Stream
{
public:

    enum class State : int
    {
        CONNECTING,
        OPEN,
        CLOSING,
        CLOSED
    };

    Stream( ) :
    state( State::CONNECTING ),
    handler( dynamic_cast<MessageHandler*> (new HandshakeHandler( )) )
    {
        DEBUG << "created";
    }

    ~Stream( )
    {
        DEBUG << "destroyed";
        if ( handler ) delete handler;
    }

    void process( )
    {
        // do not process further message in CLOSING state
        if ( state == State::CLOSING )
        {
            state = State::CLOSED;
            return;
        }

        if ( handler->process( in, out ) )
        {
            // success
            // don't need to handle SUCCESS status for OPEN
            if ( state == State::CONNECTING )
            {
                // OPENING: handshake finished
                state = State::OPEN;
                delete handler;
                handler = dynamic_cast<MessageHandler*> (new DataFrameHandler( ));
                return;
            }
        }
        else
        {
            // error
            // don't need to handle ERROR status for CONNECTING
            if ( state == State::OPEN )
            {
                // CONNECTED: got a closing frame or an invalid frame
                // response a close frame
                state = State::CLOSING;
                return;
            }
        }
    }

    SocketInStream in;
    SocketOutStream out;
    State state;
    MessageHandler* handler;
};

SocketDataHandler::SocketDataHandler( )
{
    DEBUG << "created";
}

SocketDataHandler::SocketDataHandler( SocketDataHandler&& other ) :
streams( std::move( other.streams ) )
{
    DEBUG << "moved from " << &other;
}

SocketDataHandler& SocketDataHandler::operator=(SocketDataHandler&& other)
{
    DEBUG << "moved from " << &other;
    if ( this != &other )
    {
        streams = std::move( other.streams );
    }
    return *this;
}

SocketDataHandler::~SocketDataHandler( )
{
    DEBUG << "destroyed";
}

bool SocketDataHandler::add( int socket )
{
    /* Make the incoming socket non-blocking and add it to the list of fds to monitor. */
    Event dataEvent( socket, EPOLLIN | EPOLLOUT, [this](const Event & ev)
    {
        if ( ev.isError( ) )
        {
            this->onError( ev );
            return;
        }

        if ( ev.isIn( ) )
        {
            this->onRecv( ev );
        }

        if ( ev.isOut( ) )
        {
            this->onSend( ev );
        }
    } );

    return EventLoop::getInstance( ).registerEvent( dataEvent );
}

void SocketDataHandler::onError( const Event& ev )
{
    int socket = ev.getFd( );
    DEBUG << "socket=" << socket;

    // explicitly unregister for non error event
    if ( !ev.isError( ) )
    {
        EventLoop::getInstance( ).unregisterEvent( ev );
    }

    streams.erase( socket );
}

void SocketDataHandler::onRecv( const Event& ev )
{
    int socket = ev.getFd( );
    DEBUG << "socket=" << socket;

    auto iter = streams.find( socket );
    if ( iter == streams.end( ) )
    {
        iter = streams.emplace( socket, std::unique_ptr<Stream>(new Stream( )) ).first;
    }

    if ( iter->second->state == Stream::State::CLOSING || iter->second->state == Stream::State::CLOSED )
    {
        // closing, do not process any further message, just close connection
        onError( ev );
        return;
    }

    // read data
    ssize_t count = iter->second->in.recv( socket );

    if ( count > 0 )
    {
        iter->second->process( );
        if ( !iter->second->out.empty( ) )
        {
            if ( iter->second->out.send( socket ) < 0 )
            {
                onError( ev );
            }
        }
    }
    else if ( count < 0 )
    {
        onError( ev );
    }
}

void SocketDataHandler::onSend( const Event& ev )
{
    int socket = ev.getFd( );
    DEBUG << "socket=" << socket;

    auto iter = streams.find( socket );
    if ( iter == streams.end( ) )
    {
        return;
    }

    if ( iter->second->out.send( socket ) < 0 )
    {
        onError( ev );
    }
}

