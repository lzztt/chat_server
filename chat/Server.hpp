/* 
 * File:   Server.hpp
 * Author: ikki
 *
 * Created on March 31, 2015, 10:09 PM
 */

#ifndef SERVER_HPP
#define	SERVER_HPP

#include <vector>
#include <string>
#include <memory>

#include "User.hpp"
#include "Channel.hpp"

#include "websocket/ServerApp.hpp"

namespace chat {

class Server : public websocket::ServerApp
{
public:
    Server() = default;
    
    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

    Server(Server&& other) = delete;
    Server& operator=(Server&& other) = delete;

    ~Server() = default;

    virtual void onOpen(int clientID) override;
    virtual void onClose(int clientID) override;
    virtual void onMessage(std::string msg, int clientID) override;
    virtual void onMessage(std::vector<unsigned char> msg, int clientID) override;

private:
    std::vector<std::shared_ptr<User>> users;
    std::vector<std::shared_ptr<Channel>> channels;
};

} // namespace chat

#endif	/* SERVER_HPP */

