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

SocketOutStream::Buffer::Buffer( int fd ) :
file({fd, (size_t) ::lseek( fd, 0, SEEK_END )} ),
offset( 0 )
{
    LOG_DEBUG << "buffer created with fd";
}

SocketOutStream::Buffer::Buffer( std::string&& str ) :
str( std::move( str ) ),
file({0} ),
offset( 0 )
{
    LOG_DEBUG << "buffer created with string";
}

SocketOutStream::Buffer::Buffer( SocketOutStream::Buffer&& other ) :
str( std::move( other.str ) ),
file( other.file ),
offset( other.offset )
{
    LOG_DEBUG << "moved from " << &other;
    other.file = {0};
    other.offset = 0;
}

SocketOutStream::Buffer& SocketOutStream::Buffer::operator=(SocketOutStream::Buffer&& other)
{
    LOG_DEBUG << "moved from " << &other;
    if ( this != &other )
    {
        file = other.file;
        str = std::move( other.str );
        offset = other.offset;

        other.file = {0};
        other.offset = 0;
    }

    return *this;
}

SocketOutStream::Buffer::~Buffer( )
{
    if ( file.fd ) ::close( file.fd );
}

ssize_t SocketOutStream::send( const int socket )
{
    ssize_t nSend = 0, count = 0;
    size_t nLeft = 0;
    const char* pData = nullptr;

    while ( !buffers.empty( ) )
    {
        int flags = buffers.size( ) == 1 ? 0 : MSG_MORE;
        Buffer& buf = buffers.front( );

        if ( !buf.file.fd )
        {
            // process string buffer
            pData = buf.str.data( ) + buf.offset;
            nLeft = buf.str.size( ) - buf.offset;
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
                            LOG_ERROR << "socket " << socket << " closed (" << strerror( errno ) << ")";
                            return -1;
                        }
                    }
                    else
                    {
                        // this should not happen
                        LOG_ERROR << "socket " << socket << " send 0 bytes unexpectedly";
                        return -1;
                    }
                }
            }
        }
        else
        {
            // process file buffer
            nLeft = buf.file.size - buf.offset;
            nSend += nLeft;
            while ( nLeft > 0 )
            {
                count = ::sendfile( socket, buf.file.fd, (off_t*) & buf.offset, nLeft );
                if ( count > 0 )
                {
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
                            LOG_ERROR << "socket " << socket << " closed (" << strerror( errno ) << ")";
                            return -1;
                        }
                    }
                    else
                    {
                        // this should not happen
                        LOG_ERROR << "socket " << socket << " send 0 bytes unexpectedly";
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

    LOG_DEBUG << "send " << nSend << " bytes";
    return nSend;
}
