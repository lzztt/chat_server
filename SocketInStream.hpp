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
    void pop_front( off_t count);
    bool empty();   
    void clear();
    
private:
    
    class Buffer;
    std::deque<Buffer> buffers;
};

#endif	/* SOCKETINSTREAM_HPP */

