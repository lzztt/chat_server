/* 
 * File:   MessageHandler.hpp
 * Author: ikki
 *
 * Created on March 11, 2015, 7:48 PM
 */

#ifndef MESSAGEHANDLER_HPP
#define	MESSAGEHANDLER_HPP

#include "SocketInStream.hpp"
#include "SocketOutStream.hpp"

class MessageHandler
{
public:
    explicit MessageHandler();

    MessageHandler(const MessageHandler& other) = delete;
    MessageHandler& operator=(const MessageHandler& other) = delete;

    MessageHandler(MessageHandler&& other);
    MessageHandler& operator=(MessageHandler&& other);

    virtual ~MessageHandler();

    virtual bool process(SocketInStream& in, SocketOutStream& out) = 0;
};

#endif	/* MESSAGEHANDLER_HPP */

