/* 
 * File:   main.cpp
 * Author: ikki
 *
 * Created on February 16, 2015, 10:04 PM
 */

#include <string>
#include <algorithm>

#include "websocket/ServerApp.hpp"
#include "Log.hpp"

class EchoServerApp : public websocket::ServerApp
{
public:

    EchoServerApp( ) :
    websocket::ServerApp( )
    {
    }

    virtual void onOpen( int clientID ) override
    {
        LOG_INFO << "client connected " << clientID;
    }

    virtual void onClose( int clientID ) override
    {
        LOG_INFO << "client disconnected " << clientID;
        std::vector<int>::iterator iter = std::find( clients.begin( ), clients.end( ), clientID );
        int index = std::distance( clients.begin( ), iter );
        std::string outMsg( "user left: " );
        outMsg.append( names[index] );

        clients.erase( iter );
        names.erase( names.begin( ) + index );

        send( outMsg, clients );
    }

    virtual void onMessage( std::vector<unsigned char> msg, int clientID ) override
    {
        LOG_ERROR << "not implemented";
    }

    virtual void onMessage( std::string msg, int clientID ) override
    {
        std::vector<int>::iterator i = std::find( clients.begin( ), clients.end( ), clientID );
        if ( i == clients.end( ) )
        {
            // first message to get the username
            clients.push_back( clientID );
            names.push_back( msg );
            std::string outMsg( "user connected: " );
            outMsg.append( msg );
            send( outMsg, clients );
        }
        else
        {
            // chatting messages
            int index = std::distance( clients.begin( ), i );
            std::string outMsg( names[index] );
            outMsg.append( " says: " );
            outMsg.append( msg );

            std::vector<int> toClients;
            toClients.insert( toClients.begin( ), clients.begin( ), i );
            toClients.insert( toClients.begin( ) + index, ++i, clients.end( ) );

            if ( !toClients.empty( ) ) send( outMsg, toClients );
        }
    }

private:
    std::vector<int> clients;
    std::vector<std::string> names;

};

int main( int argc, char** argv )
{
    EchoServerApp( ).run( );
    return 0;
}

