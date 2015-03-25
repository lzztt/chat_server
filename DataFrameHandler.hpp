/* 
 * File:   DataFrameHandler.hpp
 * Author: ikki
 *
 * Created on March 11, 2015, 7:53 PM
 */

#ifndef DATAFRAMEHANDLER_HPP
#define	DATAFRAMEHANDLER_HPP

#include "MessageHandler.hpp"
#include "DataFrameParser.hpp"

class DataFrameHandler : public MessageHandler
{
public:

    explicit DataFrameHandler();

    DataFrameHandler(const DataFrameHandler& other) = delete;
    DataFrameHandler& operator=(const DataFrameHandler& other) = delete;

    DataFrameHandler(DataFrameHandler&& other);
    DataFrameHandler& operator=(DataFrameHandler&& other);

    virtual ~DataFrameHandler();

    virtual bool process(SocketInStream& in, SocketOutStream& out);

private:
    bool myHandleMessage(SocketOutStream& out);
    bool myHandleFragmentFrame(SocketOutStream& out);
    void myHandleUnrealsedData(SocketOutStream& out);
    void myHandleBadMaskBit(SocketOutStream& out);
    void myHandleBadPayloadLength(SocketOutStream& out);

    void mySendCloseFrame(SocketOutStream& out);
    void mySendPongFrame(SocketOutStream& out);

    DataFrameParser myParser;

    std::string myMessage;
};

#endif	/* DATAFRAMEHANDLER_HPP */

