/* 
 * File:   DataFrameHandler.cpp
 * Author: ikki
 * 
 * Created on March 12, 2015, 9:27 PM
 */

#include "DataFrameHandler.hpp"
#include "Log.hpp"

DataFrameHandler::DataFrameHandler( )
{
}

DataFrameHandler::DataFrameHandler( DataFrameHandler&& other )
{
}

DataFrameHandler& DataFrameHandler::operator=(DataFrameHandler&& other)
{
}

DataFrameHandler::~DataFrameHandler( )
{
}

void DataFrameHandler::process(SocketInStream& in, SocketOutStream& out)
{
    DEBUG << "processing";
    //out.add( in.get( ) );
}
