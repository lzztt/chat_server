/* 
 * File:   Server.hpp
 * Author: ikki
 *
 * Created on February 20, 2015, 8:50 PM
 */

#ifndef SERVER_HPP
#define	SERVER_HPP

class Server
{
public:
    explicit Server();

    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

    Server(Server&& other) = delete;
    Server& operator=(Server&& other) = delete;

    ~Server();
};

#endif	/* SERVER_HPP */

