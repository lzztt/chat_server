/* 
 * File:   Server.cpp
 * Author: ikki
 * 
 * Created on February 20, 2015, 8:50 PM
 */

#include "Server.hpp"
#include "ServerSocket.hpp"
#include "EventLoop.hpp"

void Server::run( )
{
    ServerSocket s( 8080 );
    EventLoop::getInstance( ).run( );
}
