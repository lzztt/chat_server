/* 
 * File:   DataFrameHandler.cpp
 * Author: ikki
 * 
 * Created on March 12, 2015, 9:27 PM
 */

#include <endian.h>
#include <vector>

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
    while ( !in.empty( ) )
    {
        DataFrameParser::Header& header = myParser.getHeader( );

        switch ( myParser.parse( in ) )
        {
        case DataFrameParser::Status::SUCCESS:
            // single message
            if ( header.fin && header.opcode )
            {
                // get single message
                switch ( header.opcode )
                {
                case WS_OPCODE_TEXT:
                    // single text message
                case WS_OPCODE_BINARY:
                    // single binary message
                    myHandleMessage( out );
                    break;

                case WS_OPCODE_PING:
                    // single ping message
                    mySendPongFrame( out );
                    break;

                case WS_OPCODE_PONG:
                    // single pong message
                    // ignore for now
                    break;

                case WS_OPCODE_CLOSE:
                    // single close message
                    mySendCloseFrame( out );
                    return false;
                    break;

                default:
                    // unsupported opcode
                    mySendCloseFrame( out );
                    return false;
                    break;
                }
            }
            else
            {
                // get a message fragment
                if ( myHandleFragmentFrame( out ) == false )
                {
                    in.clear( );
                    return false;
                }
            }
            break;

        case DataFrameParser::Status::PARSING:
            break;

        case DataFrameParser::Status::UNRELEASED_DATA:
            myHandleUnrealsedData( out );
            break;

        case DataFrameParser::Status::BAD_MASK_BIT:
            myHandleBadMaskBit( out );
            in.clear( );
            return false;
            break;

        case DataFrameParser::Status::BAD_PAYLOAD_LENGTH:
            myHandleBadPayloadLength( out );
            in.clear( );
            return false;
            break;

        default:
            ERROR << "not handled parse status returned from parser";
        }
    }

    return true;
}

bool DataFrameHandler::myHandleMessage( SocketOutStream& out )
{
    std::unique_ptr<char[] > buf;
    size_t count = myParser.getData( buf );
    INFO << "get message (" << count << " bytes) : " << std::string( buf.get( ), count );

    std::string msg( buf.get( ), count );
    // msg handler call
    // return a response
    std::string resp( std::move( msg ) );

    // build response header
    size_t headerLength;
    char header[10];
    // text frame for now
    header[0] = 0x81;
    size_t dataLength = resp.size( );
    if ( dataLength < 0x7E )
    {
        headerLength = 2;
        header[1] = dataLength;
    }
    else if ( dataLength <= 0xFFFF )
    {
        headerLength = 4;
        header[1] = 0x7E;
        *((uint16_t*) (header + 2)) = htobe16( dataLength );
    }
    else if ( dataLength <= 0x7FFFFFFFFFFFFFFF )
    {
        headerLength = 10;
        header[1] = 0x7F;
        *((uint64_t*) (header + 2)) = htobe64( dataLength );
    }
    else
    {
        // too big, not handling
        ERROR << "response size if too big: " << dataLength;
    }

    out.add( std::string( header, headerLength ) );
    out.add( std::move( resp ) );
}

bool DataFrameHandler::myHandleFragmentFrame( SocketOutStream& out )
{
    DataFrameParser::Header& header = myParser.getHeader( );

    if ( myMessage.empty( ) )
    {
        // expecting the first fragment
        switch ( header.opcode )
        {
        case WS_OPCODE_TEXT:
            break;

        case WS_OPCODE_BINARY:
            // TODO: not supported
            ERROR << "BINARY frame type is not supported yet!";
            mySendCloseFrame( out );
            return false;
            break;

        default:
            // first fragment need to be TEXT or BINARY type
            mySendCloseFrame( out );
            return false;
        }
    }
    else
    {
        // expecting a CONT fragment
        if ( header.opcode != WS_OPCODE_CONTINUATION )
        {
            // following fragment need to be CONTINUATION type
            // get a wrong type, clear everything and close
            myMessage.clear( );
            mySendCloseFrame( out );
            return false;
        }
    }

    std::unique_ptr<char[] > buf;
    size_t count = myParser.getData( buf );
    myMessage.append( buf.get(), count );

    if ( header.fin )
    {
        // get the final fragment
        // TODO
    }

    return true;
}

void DataFrameHandler::myHandleUnrealsedData( SocketOutStream& out )
{
    ERROR << "get unreleased data";
    myHandleMessage( out );
}

void DataFrameHandler::myHandleBadMaskBit( SocketOutStream& out )
{
    ERROR << "bad mask bit";
    mySendCloseFrame( out );
}

void DataFrameHandler::myHandleBadPayloadLength( SocketOutStream& out )
{
    ERROR << "bad payload length";
    mySendCloseFrame( out );
}

void DataFrameHandler::mySendCloseFrame( SocketOutStream& out )
{
    INFO << "sending CLOSE frame";
    char close[2] = {(char) 0x88, (char) 0x00};
    out.add( std::string( close, 2 ) );
}

void DataFrameHandler::mySendPongFrame( SocketOutStream& out )
{
    INFO << "sending PONG frame";
    std::unique_ptr<char[] > pData;
    size_t count = myParser.getData( pData );
    if ( count == 0 )
    {
        char pong[2] = {(char) 0x8A, (char) 0x00};
        out.add( std::string( pong, 2 ) );
    }
    else
    {
        char pong[2] = {(char) 0x8A, (char) count};
        out.add( std::string( pong, 2 ) );
        out.add( std::string( pData.get( ), count ) );
    }
}

