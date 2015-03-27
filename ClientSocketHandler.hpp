/* 
 * File:   ClientSocketHandler.hpp
 * Author: ikki
 *
 * Created on February 20, 2015, 11:19 PM
 */

#ifndef CLIENTSOCKETHANDLER_HPP
#define	CLIENTSOCKETHANDLER_HPP

#include <vector>

#include "Event.hpp"
#include "EventLoop.hpp"

class WebSocketServerApp;

class ClientSocketHandler
{
public:
    explicit ClientSocketHandler(WebSocketServerApp* pServerApp);

    ClientSocketHandler(const ClientSocketHandler& other) = delete;
    ClientSocketHandler& operator=(const ClientSocketHandler& other) = delete;

    ClientSocketHandler(ClientSocketHandler&& other);
    ClientSocketHandler& operator=(ClientSocketHandler&& other);

    ~ClientSocketHandler();

    bool add(int socket);
    bool close(int socket);

protected:
    void onError(const Event& ev);
    void onRecv(const Event& ev);
    void onSend(const Event& ev);

private:

    class Stream;
    std::vector<Stream> streams;
    WebSocketServerApp* pServerApp;
};

#endif	/* CLIENTSOCKETHANDLER_HPP */

