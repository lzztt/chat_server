/* 
 * File:   HandshakeHandler.hpp
 * Author: ikki
 *
 * Created on March 11, 2015, 7:49 PM
 */

#ifndef HANDSHAKEHANDLER_HPP
#define	HANDSHAKEHANDLER_HPP

#include "MessageHandler.hpp"
#include "HandshakeParser.hpp"

class HandshakeHandler : public MessageHandler
{
public:
    explicit HandshakeHandler();

    HandshakeHandler(const HandshakeHandler& other) = delete;
    HandshakeHandler& operator=(const HandshakeHandler& other) = delete;

    HandshakeHandler(HandshakeHandler&& other);
    HandshakeHandler& operator=(HandshakeHandler&& other);

    ~HandshakeHandler();

    void process(SocketInStream& in, SocketOutStream& out);

private:
    HandshakeParser paser;
};

#endif	/* HANDSHAKEHANDLER_HPP */

