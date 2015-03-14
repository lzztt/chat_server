/* 
 * File:   DataFrameHandler.cpp
 * Author: ikki
 * 
 * Created on March 12, 2015, 9:27 PM
 */

#include "DataFrameHandler.hpp"

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
    out.add( in.get( ) );
}
