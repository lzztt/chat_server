/* 
 * File:   main.cpp
 * Author: ikki
 *
 * Created on February 16, 2015, 10:04 PM
 */

#include "Log.hpp"
#include "Exception.hpp"
#include "Event.hpp"
#include "EventLoop.hpp"
#include "Socket.hpp"

/*
 * 
 */
int main( int argc, char** argv )
{

    Socket s(8080, SocketDataHandler());
    EventLoop::getInstance().run();
    return 0;
}

