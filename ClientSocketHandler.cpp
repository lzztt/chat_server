/* 
 * File:   SocketDataHandler.cpp
 * Author: ikki
 * 
 * Created on February 20, 2015, 11:25 PM
 */

#include <sys/epoll.h>
#include <sys/socket.h>

#include "ClientSocketHandler.hpp"
#include "WebSocketServerApp.hpp"
#include "Log.hpp"
#include "SocketInStream.hpp"
#include "SocketOutStream.hpp"
#include "MessageHandler.hpp"
#include "HandshakeHandler.hpp"
#include "DataFrameHandler.hpp"

class ClientSocketHandler::Stream
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
    state( State::CLOSED ),
    handler( nullptr )
    {
        LOG_DEBUG << "created";
    }

    Stream( const Stream& other ) = delete;
    Stream& operator=(const Stream& other) = delete;

    Stream( Stream&& other ) = default;
    Stream& operator=(Stream&& other) = default;

    ~Stream( )
    {
        LOG_DEBUG << "destroyed";
        if ( handler ) delete handler;
    }

    void init( )
    {
        state = State::CONNECTING;
        if ( handler ) delete handler;
        handler = dynamic_cast<MessageHandler*> (new HandshakeHandler( ));
    }

    void open( WebSocketServerApp* pServerApp, int socket )
    {
        // OPENING: handshake finished
        state = State::OPEN;
        delete handler;
        handler = dynamic_cast<MessageHandler*> (new DataFrameHandler( pServerApp, socket ));
    }

    void close( )
    {
        if ( state != State::CLOSED )
        {
            state = State::CLOSED;
            if ( !handler )
            {
                delete handler;
                handler = nullptr;
            }
            in.clear( );
            out.clear( );
        }
    }

    SocketInStream in;
    SocketOutStream out;
    State state;
    MessageHandler* handler;
};

ClientSocketHandler::ClientSocketHandler( WebSocketServerApp* pServerApp ) :
pServerApp( pServerApp )
{
    LOG_DEBUG << "created";
}

ClientSocketHandler::ClientSocketHandler( ClientSocketHandler&& other ) :
streams( std::move( other.streams ) )
{
    LOG_DEBUG << "moved from " << &other;
}

ClientSocketHandler& ClientSocketHandler::operator=(ClientSocketHandler&& other)
{
    LOG_DEBUG << "moved from " << &other;
    if ( this != &other )
    {
        streams = std::move( other.streams );
    }
    return *this;
}

ClientSocketHandler::~ClientSocketHandler( )
{
    LOG_DEBUG << "destroyed";
}

bool ClientSocketHandler::add( int socket )
{
    // fill streams till the socket element
    if ( streams.size( ) < socket + 1 )
    {
        streams.resize( socket + 1 );
    }
    else
    {
        // close existing stream
        streams[socket].close( );
    }

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

    return pServerApp->myEventLoop.registerEvent( dataEvent );
}

bool ClientSocketHandler::close( int socket )
{
    pServerApp->myEventLoop.unregisterEvent( Event( socket, 0, [](const Event & ev)
    {
    } ) );
    streams[socket].close( );
}

void ClientSocketHandler::onError( const Event& ev )
{
    int socket = ev.getFd( );
    LOG_DEBUG << "socket=" << socket;

    // explicitly unregister for non error event
    if ( !ev.isError( ) )
    {
        pServerApp->myEventLoop.unregisterEvent( ev );
    }

    streams[socket].close( );
}

void ClientSocketHandler::onRecv( const Event& ev )
{
    int socket = ev.getFd( );
    LOG_DEBUG << "socket=" << socket;

    auto& stream = streams[socket];

    if ( stream.state == Stream::State::CLOSING )
    {
        // closing, do not process any further message, just close connection
        onError( ev );
        return;
    }

    // read data
    ssize_t count = stream.in.recv( socket );

    if ( count > 0 )
    {
        // stream.process( pServerApp, socket );
        // process
        if ( stream.state == Stream::State::CLOSED )
        {
            stream.init( );
        }

        // process message for CONNECTING and OPEN state
        // we really should only pass the in stream for message
        // for handshaking, we may pass the out stream too
        switch ( stream.handler->process( stream.in, stream.out ) )
        {
        case MessageHandler::Status::SUCCESS:
            if ( stream.state == Stream::State::CONNECTING )
            {
                stream.open( pServerApp, socket );
            }
            break;

        case MessageHandler::Status::PARSING:
            break;

        case MessageHandler::Status::ERROR:
            if ( stream.state == Stream::State::OPEN )
            {
                // CONNECTED: got a closing frame or an invalid frame
                // response a close frame

                // first need to check whether we get a pending response
                // pServerApp->sendPendingMessage( socket );

                if ( stream.out.empty( ) )
                {
                    stream.close( );
                }
                else
                {
                    // will call close() after send()
                    stream.state = Stream::State::CLOSING;
                }
            }
            break;
        }
    }
    else if ( count < 0 )
    {
        onError( ev );
    }
}

void ClientSocketHandler::onSend( const Event& ev )
{
    int socket = ev.getFd( );
    LOG_DEBUG << "socket=" << socket;

    auto& stream = streams[socket];
    if ( !stream.out.empty( ) )
    {
        if ( stream.out.send( socket ) < 0 )
        {
            onError( ev );
        }

        // close CLOSING stream, if flush the out stream
        if ( stream.out.empty( ) && stream.state == Stream::State::CLOSING )
        {
            stream.close( );
        }
    }
}

