/* 
 * File:   SocketInStream.cpp
 * Author: ikki
 * 
 * Created on March 1, 2015, 12:24 PM
 */

#include <sys/socket.h>
#include <cstring>
#include <memory>

#include "SocketInStream.hpp"
#include "Log.hpp"

#define BUFSIZE 4096
#define POOLSIZE 1000

class SocketInStream::Buffer
{
public:

    Buffer( )
    {
        DEBUG << "buffer created";
        if( pool.empty() )
        {
            pBegin = pEnd = pData = new char[BUFSIZE];
            if( !pData ) ERROR << "failed to allocate " << BUFSIZE << " bytes as buffer memory ";
        }
        else
        {
            pBegin = pEnd = pData = pool.front( ).release( );
            pool.pop_front( );
        }               
    }

    Buffer( const Buffer& other ) = delete;
    Buffer& operator=(const Buffer& other) = delete;

    Buffer( Buffer&& other ) :
    pData( other.pData ),
    pBegin( other.pBegin ),
    pEnd( other.pEnd )
    {
        DEBUG << "moved from " << &other;
        other.pBegin = other.pEnd = other.pData = nullptr;
    }

    Buffer& operator=(Buffer&& other)
    {
        DEBUG << "moved from " << &other;
        if ( this != &other )
        {
            pData = other.pData;
            pBegin = other.pBegin;
            pEnd = other.pEnd;
            other.pBegin = other.pEnd = other.pData = nullptr;
        }

        return *this;
    }

    ~Buffer( )
    {
        if ( pData )
        {
            if ( pool.size( ) < POOLSIZE )
            {
                pool.push_back( std::unique_ptr<char[]>(pData) );
            }
            else
            {
                delete[] pData;
            }
        }
    }

    void pop_front( off_t count )
    {
        pBegin += count;
        if ( pBegin > pEnd ) pBegin = pEnd;
    }

    void push_back( off_t count )
    {
        pEnd += count;
        if ( pEnd > pData + BUFSIZE ) pEnd = pData + BUFSIZE;
    }

    size_t getReadableBuffer( const char** ppBuf ) const
    {
        size_t available = pEnd - pBegin;
        *ppBuf = available ? pBegin : nullptr;
        return available;
    }

    size_t getWritableBuffer( char** ppBuf ) const
    {
        size_t left = pData + BUFSIZE - pEnd;
        *ppBuf = left ? pEnd : nullptr;
        return left;
    }

    void clear( )
    {
        pBegin = pEnd = pData;
    }

private:
    static std::deque<std::unique_ptr<char[] >> pool;

    char* pData;
    char* pBegin;
    char* pEnd;
};

// initialize static pool
std::deque<std::unique_ptr<char[] >> SocketInStream::Buffer::pool = std::deque<std::unique_ptr<char[] >>();

SocketInStream::SocketInStream( )
{
}

SocketInStream::SocketInStream( SocketInStream&& other )
{
}

SocketInStream& SocketInStream::operator=(SocketInStream&& other)
{
}

SocketInStream::~SocketInStream( )
{
}

ssize_t SocketInStream::recv( const int socket )
{
    char* pData = nullptr;
    ssize_t nRecv = 0, count = 0;
    size_t bufSize = 0, nLeft = 0;

    do
    {
        Buffer buf;
        nLeft = bufSize = buf.getWritableBuffer( &pData );

        while ( nLeft > 0 )
        {
            // read data from socket
            count = ::recv( socket, pData, nLeft, 0 );

            if ( count > 0 )
            {
                pData += count;
                nLeft -= count;
                continue;
            }
            else
            {
                if ( count == -1 )
                {
                    if ( errno == EAGAIN )
                    {
                        // no more data
                        break;
                    }
                    else
                    {
                        // recv error
                        ERROR << "socket " << socket << " closed (" << strerror( errno ) << ")";
                        return count;
                    }
                }
                else
                {
                    // socket closed
                    ERROR << "socket " << socket << " closed";
                    return -1;
                }
            }
        }

        // read some data, either reached data end or buffer end
        if ( nLeft < bufSize )
        {
            DEBUG << "recv " << bufSize - nLeft << " bytes";
            nRecv += (bufSize - nLeft);
            buf.push_back( bufSize - nLeft );
            buffers.push_back( std::move( buf ) );
        }
    }
    while ( nLeft == 0 );

    return nRecv;
}

std::string SocketInStream::get( )
{
    std::string msg;
    size_t count = 0;
    const char* pData = nullptr;
    for ( auto& b : buffers )
    {
        count = b.getReadableBuffer( &pData );
        msg.append( pData, count );
    }
    buffers.clear();

    return std::move( msg );
}
