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
    void myHandleSwitchingProtocols(SocketOutStream& out);
    void myHandleBadRequest(SocketOutStream& out);
    void myHandleMethodNotAllowed(SocketOutStream& out);
    void myHandleUpgradeRequired(SocketOutStream& out);
    void myHandleHttpVersionNotSupported(SocketOutStream& out);
    
    HandshakeParser myParser;
};

#endif	/* HANDSHAKEHANDLER_HPP */

