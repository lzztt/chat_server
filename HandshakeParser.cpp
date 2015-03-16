/* 
 * File:   HandshakeParser.cpp
 * Author: ikki
 * 
 * Created on March 12, 2015, 9:18 PM
 */

#include <cctype>

#include "HandshakeParser.hpp"
#include "Log.hpp"

HandshakeParser::HandshakeParser( ) :
myState( State::METHOD ),
myStatus( Status::PARSING ),
myMethod( Method::UNKNOWN )
{
}

HandshakeParser::HandshakeParser( HandshakeParser&& other )
{
}

HandshakeParser& HandshakeParser::operator=(HandshakeParser&& other)
{
}

HandshakeParser::~HandshakeParser( )
{
}

HandshakeParser::Status HandshakeParser::parse( SocketInStream& in )
{
    size_t nLeft = 0, count = 0;
    const char *pData = nullptr, *pBegin = nullptr, *pEnd = nullptr;
    char* pUpper = nullptr;
    
    while ( !in.empty( ) && myStatus == Status::PARSING )
    {
        nLeft = count = in.getData( &pData );
        DEBUG << std::string( pData, count );

        while ( nLeft > 0 )
        {
            switch ( myState )
            {
            case State::METHOD:
                // still unknown? find method
                if ( myMethod == Method::UNKNOWN )
                {
                    // GET, case-sensitive
                    if ( *pData++ == 'G' )
                    {
                        myMethod == Method::GET;
                    }
                    else
                    {
                        // 400 bad request
                        myStatus = Status::BAD_REQUEST;
                        in.clear( );
                        nLeft = 0;
                    }
                    --nLeft;
                }

                // move to uri, find SPACE
                for (; nLeft > 0 && *pData != ' '; ++pData, --nLeft )
                {
                    if ( *pData == '\r' || *pData == '\n' )
                    {
                        // 400 bad request
                        myStatus = Status::BAD_REQUEST;
                        in.clear( );
                        nLeft = 0;
                    }
                }
                if ( nLeft > 0 )
                {
                    ++pData;
                    --nLeft;
                    myState = State::URI;
                }
                break;

            case State::URI:
                pBegin = pData;
                // find SPACE
                for (; nLeft > 0 && *pData != ' '; ++pData, --nLeft )
                {
                    if ( *pData == '\r' || *pData == '\n' )
                    {
                        // 400 bad request
                        myStatus = Status::BAD_REQUEST;
                        in.clear( );
                        nLeft = 0;
                    }
                }

                if ( myStatus == Status::PARSING )
                {
                    myUri.append( pBegin, pData - pBegin );
                }

                if ( nLeft > 0 )
                {
                    ++pData;
                    --nLeft;
                    myState = State::HTTP_VERSION;
                }
                break;

            case State::HTTP_VERSION:
                pBegin = pData;
                // find CRLF
                for (; nLeft > 0 && *pData != '\r'; ++pData, --nLeft )
                {
                }
                myHttpVersion.append( pBegin, pData - pBegin );

                if ( nLeft > 0 )
                {
                    if ( myHttpVersion != "HTTP/1.1" )
                    {
                        // 400 bad request
                        // log bad http version
                        myStatus = Status::BAD_REQUEST;
                        in.clear( );
                        nLeft = 0;
                    }

                    if ( nLeft >= 2 )
                    {
                        pData += 2;
                        nLeft -= 2;
                        myState = State::HEADER_NAME;
                    }
                    else
                    {
                        // '\r' is at buffer end
                        myState = State::CRLF;
                        nLeft = 0;
                    }
                }
                break;

            case State::CRLF:
                // skip '\n'
                ++pData;
                --nLeft;

                if ( nLeft > 0 )
                {
                    if ( *pData != '\r' )
                    {
                        myState = State::HEADER_NAME;
                    }
                    else
                    {
                        // headers end
                        if ( nLeft >= 2 )
                        {
                            // pData += 2;
                            nLeft -= 2;
                            myState = State::END;
                        }
                        else
                        {
                            // '\r' is at buffer end
                            myState = State::ALMOST_END;
                            nLeft = 0;
                        }
                    }
                }
                else
                {
                    myState = State::HEADER_NAME;
                }
                break;

            case State::HEADER_NAME:
                if ( *pData != '\r' )
                {
                    pBegin = pData;
                    // find COLON
                    pUpper = const_cast<char*> (pData);
                    for (; nLeft > 0 && *pData != ':'; ++pData, ++pUpper, --nLeft )
                    {
                        if ( *pData == '\r' || *pData == '\n' )
                        {
                            // 400 bad request
                            myStatus = Status::BAD_REQUEST;
                            in.clear( );
                            nLeft = 0;
                        }
                        else
                        {
                            // convert to upper case
                            *pUpper = std::toupper( *pData );
                        }
                    }

                    if ( myStatus == Status::PARSING )
                    {
                        myCurrentHeaderName.append( pBegin, pData - pBegin );
                    }

                    if ( nLeft > 0 )
                    {
                        ++pData;
                        --nLeft;
                        myState = State::HEADER_VALUE;
                    }
                }
                else
                {
                    // headers end
                    if ( nLeft >= 2 )
                    {
                        // pData += 2;
                        nLeft -= 2;
                        myState = State::END;
                    }
                    else
                    {
                        // '\r' is at buffer end
                        myState = State::ALMOST_END;
                        nLeft = 0;
                    }
                }

                break;

            case State::HEADER_VALUE:
                // skip optional white spaces
                for (; nLeft > 0 && *pData == ' '; ++pData, --nLeft )
                {
                    if ( *pData == '\r' || *pData == '\n' )
                    {
                        // 400 bad request
                        myStatus = Status::BAD_REQUEST;
                        in.clear( );
                        nLeft = 0;
                    }
                }

                if ( myStatus == Status::PARSING )
                {
                    pBegin = pData;
                    // find CRLF
                    for (; nLeft > 0 && *pData != '\r'; ++pData, --nLeft )
                    {
                    }
                    // found value end
                    if ( nLeft > 0 )
                    {
                        // remove trailing white spaces
                        for ( pEnd = pData; *pEnd == ' ' && pEnd > pBegin; --pEnd )
                        {
                        }

                        if ( pEnd > pBegin )
                        {
                            // get some value data
                            myCurrentHeaderValue.append( pBegin, pEnd - pBegin );
                        }
                        else
                        {
                            // all white spaces, further trailing right white spaces
                            size_t length = myCurrentHeaderValue.size( );
                            if ( length > 0 )
                            {
                                size_t found = myCurrentHeaderValue.find_last_not_of( " \t" );
                                if ( found != std::string::npos )
                                {
                                    if ( found < length - 1 )
                                    {
                                        myCurrentHeaderValue.erase( found + 1 );
                                    }
                                }
                                else
                                {
                                    myCurrentHeaderValue.clear( ); // all white spaces
                                }
                            }
                        }

                        if ( !myCurrentHeaderValue.empty( ) )
                        {
                            myHeaders.emplace( myCurrentHeaderName, myCurrentHeaderValue );
                        }
                        myCurrentHeaderName.clear( );
                        myCurrentHeaderValue.clear( );
                        if ( nLeft >= 2 )
                        {
                            pData += 2;
                            nLeft -= 2;
                            myState = State::HEADER_NAME;
                        }
                        else
                        {
                            // '\r' is at buffer end
                            myState = State::CRLF;
                            nLeft = 0;
                        }
                    }
                    else
                    {
                        // not found value end, just append current data
                        myCurrentHeaderValue.append( pBegin, pData - pBegin );
                    }
                }
                break;

            case State::ALMOST_END:
                // skip '\n'
                --nLeft;

                myState = State::END;
                in.pop_front( count - nLeft );
                nLeft = 0;
                break;

            case State::END:
                in.pop_front( count - nLeft );
                nLeft = 0;
                break;
            }
        }

        if ( myState == State::END )
        {
            break;
        }
        else
        {
            // continue to load next buffer
            in.pop_front( count );
        }
    }

    if ( myState == State::END )
    {
        // validate |Host| header

        // validate |Upgrade| header

        // validate |Connection| header

        // validate |Origin| header

        // validate |Sec-WebSocket-Version| header

        // validate |Sec-WebSocket-Key| header

        // check optional |Sec-WebSocket-Protocol| header

        // check optional |Sec-WebSocket-Extensions| header

        myStatus = Status::OK;
        INFO << "URI:\t" << myUri;
        for ( auto iter = myHeaders.begin( ), iterE = myHeaders.end( ); iter != iterE; ++iter )
        {
            INFO << "HEADER\t" << iter->first << "\t:\t" << iter->second;
        }
    }

    return myStatus;
}
