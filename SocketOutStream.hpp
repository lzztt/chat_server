/* 
 * File:   SocketOutStream.hpp
 * Author: ikki
 *
 * Created on March 1, 2015, 12:24 PM
 */

#ifndef SOCKETOUTSTREAM_HPP
#define	SOCKETOUTSTREAM_HPP

#include <deque>
#include <string>

class SocketOutStream
{
public:
    explicit SocketOutStream();

    SocketOutStream(const SocketOutStream& other) = delete;
    SocketOutStream& operator=(const SocketOutStream& other) = delete;

    SocketOutStream(SocketOutStream&& other);
    SocketOutStream& operator=(SocketOutStream&& other);

    ~SocketOutStream();
    
    ssize_t send(const int socket);
    
    void add( std::string&& str );
    void add( int fd );    

private:
    class Buffer;
    std::deque<Buffer> buffers;
};

#endif	/* SOCKETOUTSTREAM_HPP */

