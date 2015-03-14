/* 
 * File:   SocketOutStream.cpp
 * Author: ikki
 * 
 * Created on March 1, 2015, 12:24 PM
 */

#include <sys/socket.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <cstring>
#include <memory>

#include "SocketOutStream.hpp"
#include "Log.hpp"

class SocketOutStream::Buffer
{
public:

    Buffer( int fd ) :
    offset( 0 ),
    str( nullptr ),
    fd( fd )
    {
        DEBUG << "buffer created with fd";
    }

    Buffer( std::string&& str ) :
    offset( 0 ),
    str( nullptr ),
    fd( 0 )
    {
        DEBUG << "buffer created with string";
        this->str = std::unique_ptr<std::string>(new std::string( str ));
    }

    Buffer( const Buffer& other ) = delete;
    Buffer& operator=(const Buffer& other) = delete;

    Buffer( Buffer&& other ) :
    offset( other.offset ),
    str( std::move( other.str ) ),
    fd( other.fd )
    {
        DEBUG << "moved from " << &other;
        other.offset = 0;
        other.str = nullptr;
        other.fd = 0;
    }

    Buffer& operator=(Buffer&& other)
    {
        DEBUG << "moved from " << &other;
        if ( this != &other )
        {
            offset = other.offset;
            str = std::move( other.str );
            fd = other.fd;
            other.offset = 0;
            other.str = nullptr;
            other.fd = 0;
        }

        return *this;
    }

    ~Buffer( )
    {
        if ( fd ) ::close( fd );
    }

    off_t offset;
    std::unique_ptr<std::string> str;
    int fd;
};

SocketOutStream::SocketOutStream( )
{
}

SocketOutStream::SocketOutStream( SocketOutStream&& other )
{
}

SocketOutStream& SocketOutStream::operator=(SocketOutStream&& other)
{
}

SocketOutStream::~SocketOutStream( )
{
}

ssize_t SocketOutStream::send( const int socket )
{
    if ( buffers.empty( ) )
    {
        return 0;
    }

    ssize_t nSend = 0, count = 0;
    size_t nLeft = 0;
    const char* pData = nullptr;

    while ( !buffers.empty( ) )
    {
        int flags = buffers.size( ) == 1 ? 0 : MSG_MORE;
        Buffer& buf = buffers.front( );

        if ( buf.str )
        {
            // process string buffer
            pData = buf.str->data( ) + buf.offset;
            nLeft = buf.str->size( ) - buf.offset;
            nSend += nLeft;
            while ( nLeft > 0 )
            {
                // send the string buffer
                count = ::send( socket, pData, nLeft, flags );
                if ( count > 0 )
                {
                    pData += count;
                    buf.offset += count;
                    nLeft -= count;
                    continue;
                }
                else
                {
                    if ( count == -1 )
                    {
                        if ( errno == EAGAIN )
                        {
                            // no more send
                            break;
                        }
                        else
                        {
                            // send error
                            ERROR << "socket " << socket << " closed (" << strerror( errno ) << ")";
                            return -1;
                        }
                    }
                    else
                    {
                        // this should not happen
                        ERROR << "socket " << socket << " send 0 bytes unexpectedly";
                        return -1;
                    }
                }
            }
        }
        else
        {
            // process file buffer
            nLeft = ::lseek( buf.fd, 0, SEEK_END ) - buf.offset;
            nSend += nLeft;
            while ( nLeft > 0 )
            {
                count = ::sendfile( socket, buf.fd, (off_t*) & buf.offset, nLeft );
                if ( count > 0 )
                {
                    nLeft -= count;
                    continue;
                }
                else
                {
                    if ( count == -1 )
                    {
                        if ( errno == EAGAIN )
                        {
                            // no more send
                            break;
                        }
                        else
                        {
                            // send error
                            ERROR << "socket " << socket << " closed (" << strerror( errno ) << ")";
                            return -1;
                        }
                    }
                    else
                    {
                        // this should not happen
                        ERROR << "socket " << socket << " send 0 bytes unexpectedly";
                        return -1;
                    }
                }
            }
        }

        if ( nLeft == 0 )
        {
            buffers.pop_front( );
        }
        else
        {
            nSend -= nLeft;
            break;
        }
    }

    return nSend;
}

void SocketOutStream::add( std::string&& str )
{
    buffers.push_back( Buffer( std::move( str ) ) );
}

void SocketOutStream::add( int fd )
{
    buffers.push_back( Buffer( fd ) );
}


