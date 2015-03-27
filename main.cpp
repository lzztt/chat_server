/* 
 * File:   main.cpp
 * Author: ikki
 *
 * Created on February 16, 2015, 10:04 PM
 */

#include "WebSocketServerApp.hpp"

class EchoServerApp : public WebSocketServerApp
{

    virtual void onOpen( int clientID ) override
    {

    }

    virtual void onClose( int clientID ) override
    {

    }

    virtual void onMessage( std::vector<unsigned char> msg, int clientID ) override
    {
        send( msg, clientID );
    }

    virtual void onMessage( std::string msg, int clientID ) override
    {
        send( msg, clientID );
    }

};

int main( int argc, char** argv )
{

    EchoServerApp( ).run( );
    return 0;
}

