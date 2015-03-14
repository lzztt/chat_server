/* 
 * File:   SocketDataHandler.hpp
 * Author: ikki
 *
 * Created on February 20, 2015, 11:19 PM
 */

#ifndef SOCKETDATAHANDLER_HPP
#define	SOCKETDATAHANDLER_HPP

#include <unordered_map>
#include <memory>

#include "Event.hpp"

class SocketDataHandler
{
public:
    explicit SocketDataHandler();

    SocketDataHandler(const SocketDataHandler& other) = delete;
    SocketDataHandler& operator=(const SocketDataHandler& other) = delete;

    SocketDataHandler(SocketDataHandler&& other);
    SocketDataHandler& operator=(SocketDataHandler&& other);

    ~SocketDataHandler();

    bool add(int socket);

protected:
    void onError(const Event& ev);
    void onRecv(const Event& ev);
    void onSend(const Event& ev);

private:

    class Stream;
    std::unordered_map<int, std::unique_ptr<Stream>> streams;
};

#endif	/* SOCKETDATAHANDLER_HPP */

