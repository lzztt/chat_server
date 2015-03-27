/* 
 * File:   SocketInStream.cpp
 * Author: ikki
 * 
 * Created on March 1, 2015, 12:24 PM
 */

#include <sys/socket.h>
#include <cstring>

#include "SocketInStream.hpp"
#include "Log.hpp"

#define BUFSIZE 4096
#define POOLSIZE 1000

// initialize static pool
std::deque<std::unique_ptr<char[] >> SocketInStream::Buffer::pool = std::deque<std::unique_ptr<char[] >>();

SocketInStream::Buffer::Buffer( )
{
    LOG_DEBUG << "buffer created";
    if ( !pool.empty( ) )
    {
        pBegin = pEnd = pData = pool.front( ).release( );
        pool.pop_front( );
    }
    else
    {
        pBegin = pEnd = pData = new char[BUFSIZE];
        if ( !pData ) LOG_ERROR << "failed to allocate " << BUFSIZE << " bytes as buffer memory ";
    }
}

SocketInStream::Buffer::Buffer( SocketInStream::Buffer&& other ) :
pData( other.pData ),
pBegin( other.pBegin ),
pEnd( other.pEnd )
{
    LOG_DEBUG << "moved from " << &other;
    other.pBegin = other.pEnd = other.pData = nullptr;
}

SocketInStream::Buffer& SocketInStream::Buffer::operator=(SocketInStream::Buffer&& other)
{
    LOG_DEBUG << "moved from " << &other;
    if ( this != &other )
    {
        pData = other.pData;
        pBegin = other.pBegin;
        pEnd = other.pEnd;
        other.pBegin = other.pEnd = other.pData = nullptr;
    }

    return *this;
}

SocketInStream::Buffer::~Buffer( )
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

size_t SocketInStream::Buffer::getDataSize( ) const
{
    return pEnd - pBegin;
}

size_t SocketInStream::Buffer::getFreeCapacity( ) const
{
    return pData + BUFSIZE - pEnd;
}

void SocketInStream::Buffer::pop_front( off_t count )
{
    pBegin += count;
    if ( pBegin > pEnd ) pBegin = pEnd;
}

void SocketInStream::Buffer::push_back( off_t count )
{
    pEnd += count;
    if ( pEnd > pData + BUFSIZE ) pEnd = pData + BUFSIZE;
}

size_t SocketInStream::Buffer::getReadableBuffer( const char** ppBuf ) const
{
    size_t available = pEnd - pBegin;
    *ppBuf = available ? pBegin : nullptr;
    return available;
}

size_t SocketInStream::Buffer::getWritableBuffer( char** ppBuf ) const
{
    size_t left = pData + BUFSIZE - pEnd;
    *ppBuf = left ? pEnd : nullptr;
    return left;
}

void SocketInStream::Buffer::clear( )
{
    pBegin = pEnd = pData;
}

ssize_t SocketInStream::recv( const int socket )
{
    char* pData = nullptr;
    ssize_t nRecv = 0, count = 0;
    size_t bufSize = 0, nLeft = 0, nWriten = 0;

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
                        LOG_ERROR << "socket " << socket << " closed (" << strerror( errno ) << ")";
                        return count;
                    }
                }
                else
                {
                    // socket closed
                    LOG_ERROR << "socket " << socket << " closed";
                    return -1;
                }
            }
        }

        // read some data, either reached data end or buffer end
        if ( nLeft < bufSize )
        {
            nWriten = bufSize - nLeft;
            LOG_DEBUG << "recv " << nWriten << " bytes";
            nRecv += nWriten;
            buf.push_back( nWriten );
            buffers.push_back( std::move( buf ) );
            mySize += nWriten;
        }
    }
    while ( nLeft == 0 );

    return nRecv;
}

size_t SocketInStream::getData( const char** ppBuffer )
{
    if ( !buffers.empty( ) )
    {
        return buffers.front( ).getReadableBuffer( ppBuffer );
    }
    else
    {
        *ppBuffer == nullptr;
        return 0;
    }
}

void SocketInStream::pop_front( off_t count )
{
    if ( count >= mySize )
    {
        buffers.clear( );
        mySize = 0;
        return;
    }

    // adjust size
    mySize -= count;
    // pop_front buffer
    size_t size = 0;
    while ( count > 0 )
    {
        size = buffers.front( ).getDataSize( );
        if ( size <= count )
        {
            buffers.pop_front( );
            count -= size;
        }
        else
        {
            buffers.front( ).pop_front( count );
            count = 0;
        }
    }
}

bool SocketInStream::extract( char* buf, size_t count )
{
    if ( count > mySize )
    {
        return false;
    }

    // reduce the size
    mySize -= count;

    // extract data
    const char* pData = nullptr;
    size_t size = 0;
    while ( count > 0 )
    {
        size = buffers.front( ).getReadableBuffer( &pData );
        if ( size <= count )
        {
            std::memcpy( buf, pData, size );
            buffers.pop_front( );
            count -= size;
        }
        else
        {
            std::memcpy( buf, pData, count );
            buffers.front( ).pop_front( count );
            count = 0;
        }
    }

    return true;
}

bool SocketInStream::maskExtract( char* buf, size_t count, const char* mask, size_t maskCount, size_t maskStart )
{
    if ( count > mySize )
    {
        return false;
    }

    // reduce the size
    mySize -= count;

    // extract data
    const char* pData = nullptr;
    size_t size = 0;
    while ( count > 0 )
    {
        size = buffers.front( ).getReadableBuffer( &pData );
        if ( size <= count )
        {
            int i = maskStart, n = size + maskStart;
            for (; i < n; ++i, ++pData, ++buf )
            {
                *buf = (*pData ^ mask[i % maskCount]);
            }
            maskStart = i % maskCount;
            buffers.pop_front( );
            count -= size;
        }
        else
        {
            int i = maskStart, n = count + maskStart;
            for (; i < n; ++i, ++pData, ++buf )
            {
                *buf = (*pData ^ mask[i % maskCount]);
            }
            maskStart = i % maskCount;
            buffers.front( ).pop_front( count );
            count = 0;
        }
    }

    return true;
}
