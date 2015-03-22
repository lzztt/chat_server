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

bool DataFrameHandler::process( SocketInStream& in, SocketOutStream& out )
{
    switch ( myParser.parse( in ) )
    {
    case DataFrameParser::Status::SUCCESS:
    {
        std::unique_ptr<char[] > buf;
        size_t count = myParser.getData( buf );
        INFO << "get message (" << count << " bytes) : " << std::string( buf.get( ), count );
    }
        break;

    case DataFrameParser::Status::MISSING_MASK_BIT:
        break;

    case DataFrameParser::Status::PARSING:
        break;

    default:
        ERROR << "not handled parse status returned from parser";
    }

    return true;
    //out.add( in.get( ) );
}
