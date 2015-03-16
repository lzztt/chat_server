/* 
 * File:   HandshakeHandler.cpp
 * Author: ikki
 * 
 * Created on March 12, 2015, 9:27 PM
 */

#include "HandshakeHandler.hpp"
#include "Log.hpp"

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
    paser.parse( in );
    //out.add( in.get( ) );
}
