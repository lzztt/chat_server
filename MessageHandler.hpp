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

    enum class Status : int
    {
        PARSING,
        SUCCESS,
        ERROR
    };

    MessageHandler() = default;

    MessageHandler(const MessageHandler& other) = delete;
    MessageHandler& operator=(const MessageHandler& other) = delete;

    MessageHandler(MessageHandler&& other) = delete;
    MessageHandler& operator=(MessageHandler&& other) = delete;

    virtual ~MessageHandler() = default;

    virtual Status process(SocketInStream& in, SocketOutStream& out) = 0;
};

#endif	/* MESSAGEHANDLER_HPP */

