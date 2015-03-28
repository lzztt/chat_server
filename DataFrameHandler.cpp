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

MessageHandler::Status DataFrameHandler::process( SocketInStream& in, SocketOutStream& out )
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
                    return Status::ERROR;
                    break;

                default:
                    // unsupported opcode
                    mySendCloseFrame( out );
                    return Status::ERROR;
                    break;
                }
            }
            else
            {
                // get a message fragment
                if ( myHandleFragmentFrame( out ) == false )
                {
                    return Status::ERROR;
                }
            }
            break;

        case DataFrameParser::Status::PARSING:
            Status::PARSING;
            break;

        case DataFrameParser::Status::UNRELEASED_DATA:
            myHandleUnrealsedData( out );
            break;

        case DataFrameParser::Status::BAD_MASK_BIT:
            myHandleBadMaskBit( out );
            return Status::ERROR;
            break;

        case DataFrameParser::Status::BAD_PAYLOAD_LENGTH:
            myHandleBadPayloadLength( out );
            in.clear( );
            return Status::ERROR;
            break;

        default:
            LOG_ERROR << "not handled parse status returned from parser";
        }
    }

    return Status::SUCCESS;
}

bool DataFrameHandler::myHandleMessage( SocketOutStream& out )
{
    std::unique_ptr<unsigned char[] > buf;
    size_t count = myParser.getData( buf );
    LOG_INFO << "get message (" << count << " bytes) : " << std::string( (char*) buf.get( ), count );

    std::string msg( (char*) buf.get( ), count );
    // pass to the onMessage event handler
    pServerApp->onMessage( msg, myClientID );
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
            LOG_ERROR << "BINARY frame type is not supported yet!";
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

    std::unique_ptr<unsigned char[] > buf;
    size_t count = myParser.getData( buf );
    myMessage.append( (char*) buf.get( ), count );

    if ( header.fin )
    {
        // get the final fragment
        // pass to the onMessage event handler
        pServerApp->onMessage( myMessage, myClientID );
    }

    return true;
}

void DataFrameHandler::myHandleUnrealsedData( SocketOutStream& out )
{
    LOG_ERROR << "get unreleased data";
    myHandleMessage( out );
}

void DataFrameHandler::myHandleBadMaskBit( SocketOutStream& out )
{
    LOG_ERROR << "bad mask bit";
    mySendCloseFrame( out );
}

void DataFrameHandler::myHandleBadPayloadLength( SocketOutStream& out )
{
    LOG_ERROR << "bad payload length";
    mySendCloseFrame( out );
}

void DataFrameHandler::mySendCloseFrame( SocketOutStream& out )
{
    LOG_INFO << "sending CLOSE frame";
    unsigned char close[2] = {(unsigned char) 0x88, (unsigned char) 0x00};
    out.add( std::vector<unsigned char>( close, close + 2 ) );
}

void DataFrameHandler::mySendPongFrame( SocketOutStream& out )
{
    LOG_INFO << "sending PONG frame";
    std::unique_ptr<unsigned char[] > pData;
    size_t count = myParser.getData( pData );
    if ( count == 0 )
    {
        unsigned char pong[2] = {(unsigned char) 0x8A, (unsigned char) 0x00};
        out.add( std::vector<unsigned char>( pong, pong + 2 ) );
    }
    else
    {
        unsigned char pong[2] = {(unsigned char) 0x8A, (unsigned char) count};
        out.add( std::vector<unsigned char>( pong, pong + 2 ) );
        out.add( std::vector<unsigned char>( pData.get( ), pData.get( ) + count ) );
    }
}

