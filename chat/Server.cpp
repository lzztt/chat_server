/* 
 * File:   Server.cpp
 * Author: ikki
 * 
 * Created on March 31, 2015, 10:10 PM
 */

#include <algorithm>

#include "Server.hpp"
#include "Log.hpp"

namespace chat {

void Server::onOpen( int clientID )
{
    LOG_INFO << "client connected " << clientID;
}

void Server::onClose( int clientID )
{
    LOG_INFO << "client disconnected " << clientID;
    if( clientID > 0 && clientID < (int) users.size() )
    {
        // notify channel;
        
        // delete user;
        users[clientID].reset();
    }
    /*
    std::vector<int>::iterator iter = std::find( clients.begin( ), clients.end( ), clientID );
    if ( iter != clients.end( ) )
    {
        int index = std::distance( clients.begin( ), iter );
        std::string outMsg( "user left: " );
        outMsg.append( names[index] );

        clients.erase( iter );
        names.erase( names.begin( ) + index );

        send( outMsg, clients );
    }
     */
}

void Server::onMessage( std::string msg, int clientID )
{
    /*
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
     */
}

void Server::onMessage( std::vector<unsigned char> msg, int clientID )
{
    LOG_ERROR << "not implemented";
}

} // namespace chat