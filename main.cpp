/* 
 * File:   main.cpp
 * Author: ikki
 *
 * Created on February 16, 2015, 10:04 PM
 */

#include "WebSocketServerApp.hpp"
#include "Log.hpp"

class EchoServerApp : public WebSocketServerApp
{

    virtual void onOpen( int clientID ) override
    {
        LOG_INFO << "client connected " << clientID;
    }

    virtual void onClose( int clientID ) override
    {
        LOG_INFO << "client disconnected " << clientID;
    }

    virtual void onMessage( std::vector<unsigned char> msg, int clientID ) override
    {
        LOG_ERROR << "not implemented";
    }

    virtual void onMessage( std::string msg, int clientID ) override
    {
        msg.append(" -_-!!");
        send( msg, clientID );
    }

};

int main( int argc, char** argv )
{

    EchoServerApp( ).run( );
    return 0;
}

