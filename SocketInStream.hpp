/* 
 * File:   SocketInStream.hpp
 * Author: ikki
 *
 * Created on March 1, 2015, 12:24 PM
 */

#ifndef SOCKETINSTREAM_HPP
#define	SOCKETINSTREAM_HPP

#include <deque>
#include <string>

class SocketInStream
{
public:
    explicit SocketInStream();

    SocketInStream(const SocketInStream& other) = delete;
    SocketInStream& operator=(const SocketInStream& other) = delete;

    SocketInStream(SocketInStream&& other);
    SocketInStream& operator=(SocketInStream&& other);

    ~SocketInStream();

    ssize_t recv(const int socket);

    size_t getData(const char** ppBuffer);
    void pop_front(off_t count);

    size_t size()
    {
        return mySize;
    }

    bool empty()
    {
        return mySize == 0;
    }

    void clear();

    bool extract(char* buf, size_t count);

    bool maskExtract(char* buf, size_t count, const char* mask, size_t maskCount, size_t maskStart);

private:

    class Buffer;
    size_t mySize;
    std::deque<Buffer> buffers;
};

#endif	/* SOCKETINSTREAM_HPP */

