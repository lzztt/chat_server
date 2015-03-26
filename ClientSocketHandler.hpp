/* 
 * File:   ClientSocketHandler.hpp
 * Author: ikki
 *
 * Created on February 20, 2015, 11:19 PM
 */

#ifndef CLIENTSOCKETHANDLER_HPP
#define	CLIENTSOCKETHANDLER_HPP

#include <unordered_map>
#include <memory>

#include "Event.hpp"
#include "EventLoop.hpp"

class ClientSocketHandler
{
public:
    explicit ClientSocketHandler(EventLoop* pEventLoop);

    ClientSocketHandler(const ClientSocketHandler& other) = delete;
    ClientSocketHandler& operator=(const ClientSocketHandler& other) = delete;

    ClientSocketHandler(ClientSocketHandler&& other);
    ClientSocketHandler& operator=(ClientSocketHandler&& other);

    ~ClientSocketHandler();

    bool add(int socket);
    bool remove(int socket);

protected:
    void onError(const Event& ev);
    void onRecv(const Event& ev);
    void onSend(const Event& ev);

private:

    class Stream;
    std::unordered_map<int, std::unique_ptr<Stream>> streams;
    EventLoop* pEventLoop;
};

#endif	/* CLIENTSOCKETHANDLER_HPP */

