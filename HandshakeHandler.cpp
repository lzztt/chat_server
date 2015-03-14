/* 
 * File:   HandshakeHandler.cpp
 * Author: ikki
 * 
 * Created on March 12, 2015, 9:27 PM
 */

#include "HandshakeHandler.hpp"

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

void HandshakeHandler::process(SocketInStream& in, SocketOutStream& out)
{
    out.add( in.get( ) );
}
