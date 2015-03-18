/* 
 * File:   HandshakeHandler.cpp
 * Author: ikki
 * 
 * Created on March 12, 2015, 9:27 PM
 */

#include "HandshakeHandler.hpp"
#include "Log.hpp"
#include "sha1/sha1.hpp"
#include "base64/base64.hpp"

HandshakeHandler::HandshakeHandler( )
{
}

HandshakeHandler::HandshakeHandler( HandshakeHandler&& other )
{
}

HandshakeHandler& HandshakeHandler::operator=(HandshakeHandler&& other)
{
}

HandshakeHandler::~HandshakeHandler( )
{
}

void HandshakeHandler::process( SocketInStream& in, SocketOutStream& out )
{
    DEBUG << "processing";
    HandshakeParser::Status status = myParser.parse( in );
    switch ( status )
    {
    case HandshakeParser::Status::SWITCHING_PROTOCOLS:
        myHandleSwitchingProtocols( out );
        break;

    case HandshakeParser::Status::BAD_REQUEST:
        myHandleBadRequest( out );
        break;

    case HandshakeParser::Status::METHOD_NOT_ALLOWED:
        myHandleMethodNotAllowed( out );
        break;

    case HandshakeParser::Status::UPGRADE_REQUIRED:
        myHandleUpgradeRequired( out );
        break;

    case HandshakeParser::Status::HTTP_VERSION_NOT_SUPPORTED:
        myHandleHttpVersionNotSupported( out );
        break;

    case HandshakeParser::Status::PARSING:
        break;

    default:
        ERROR << "unsupported status";
    }
}

void HandshakeHandler::myHandleSwitchingProtocols( SocketOutStream& out )
{
    std::string key = myParser.getWebSocketKey( );
    key.append( "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" );
    std::string response = std::string( "HTTP/1.1 101 Switching Protocols\r\n"
                                        "Upgrade: websocket\r\n"
                                        "Connection: Upgrade\r\n"
                                        "Sec-WebSocket-Accept: " );
    response.append( base64::encode( sha1::encode( key ) ) ).append( "\r\n\r\n" );
    out.add( std::move( response ) );
}

void HandshakeHandler::myHandleBadRequest( SocketOutStream& out )
{
    out.add( "HTTP/1.1 400 Bad Request\r\n\r\n" );
}

void HandshakeHandler::myHandleMethodNotAllowed( SocketOutStream& out )
{
    out.add( "HTTP/1.1 405 Method Not Allowed\r\n\r\n" );
}

void HandshakeHandler::myHandleUpgradeRequired( SocketOutStream& out )
{
    out.add( "HTTP/1.1 426 Upgrade Required\r\n\r\n" );
}

void HandshakeHandler::myHandleHttpVersionNotSupported( SocketOutStream& out )
{
    out.add( "HTTP/1.1 505 HTTP Version Not Supported\r\n\r\n" );
}


