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

    SocketOutStream()
    {
    }

    SocketOutStream(const SocketOutStream& other) = delete;
    SocketOutStream& operator=(const SocketOutStream& other) = delete;

    SocketOutStream(SocketOutStream&& other) :
    buffers(std::move(other.buffers))
    {
    }

    SocketOutStream& operator=(SocketOutStream&& other)
    {
        if (this != &other)
        {
            buffers = std::move(other.buffers);
        }
        return *this;
    }

    ~SocketOutStream() = default;

    void add(std::string&& str)
    {
        buffers.push_back(Buffer(std::move(str)));
    }

    void add(int fd)
    {
        buffers.push_back(Buffer(fd));
    }

    bool empty()
    {
        return buffers.empty();
    }

    void clear()
    {
        buffers.clear();
    }
    
    ssize_t send(const int socket);

private:

    class Buffer
    {
    public:

        struct File
        {
            int fd;
            size_t size;
        };

        Buffer(int fd);
        Buffer(std::string&& str);
        Buffer(const Buffer& other) = delete;
        Buffer& operator=(const Buffer& other) = delete;
        Buffer(Buffer&& other);
        Buffer& operator=(Buffer&& other);
        ~Buffer();

        std::string str;
        File file;
        off_t offset;
    };

    std::deque<Buffer> buffers;
};

#endif	/* SOCKETOUTSTREAM_HPP */

