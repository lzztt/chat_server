/* 
 * File:   DataFrameHandler.hpp
 * Author: ikki
 *
 * Created on March 11, 2015, 7:53 PM
 */

#ifndef DATAFRAMEHANDLER_HPP
#define	DATAFRAMEHANDLER_HPP

#include "MessageHandler.hpp"

class DataFrameHandler: public MessageHandler
{
public:
    explicit DataFrameHandler();

    DataFrameHandler(const DataFrameHandler& other) = delete;
    DataFrameHandler& operator=(const DataFrameHandler& other) = delete;

    DataFrameHandler(DataFrameHandler&& other);
    DataFrameHandler& operator=(DataFrameHandler&& other);

    ~DataFrameHandler();
    
    void process(SocketInStream& in, SocketOutStream& out);
};

#endif	/* DATAFRAMEHANDLER_HPP */

