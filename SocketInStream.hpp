/* 
 * File:   SocketInStream.hpp
 * Author: ikki
 *
 * Created on March 1, 2015, 12:24 PM
 */

#ifndef SOCKETINSTREAM_HPP
#define	SOCKETINSTREAM_HPP

#include <deque>
#include <memory>

class SocketInStream
{
public:

    SocketInStream() :
    mySize(0)
    {
    }

    SocketInStream(const SocketInStream& other) = delete;
    SocketInStream& operator=(const SocketInStream& other) = delete;

    SocketInStream(SocketInStream&& other) :
    buffers(std::move(other.buffers))
    {
    }

    SocketInStream& operator=(SocketInStream&& other)
    {
        if (this != &other)
        {
            buffers = std::move(other.buffers);
        }
        return *this;
    }

    ~SocketInStream() = default;

    size_t size()
    {
        return mySize;
    }

    bool empty()
    {
        return mySize == 0;
    }

    void clear()
    {
        buffers.clear();
        mySize = 0;
    }

    ssize_t recv(const int socket);
    size_t getData(const char** ppBuffer);
    void pop_front(off_t count);
    bool extract(char* buf, size_t count);
    bool maskExtract(char* buf, size_t count, const char* mask, size_t maskCount, size_t maskStart);

private:

    class Buffer
    {
    public:

        Buffer();
        Buffer(const Buffer& other) = delete;
        Buffer& operator=(const Buffer& other) = delete;
        Buffer(Buffer&& other);
        Buffer& operator=(Buffer&& other);
        ~Buffer();

        size_t getDataSize() const;
        size_t getFreeCapacity() const;
        void pop_front(off_t count);
        void push_back(off_t count);
        size_t getReadableBuffer(const char** ppBuf) const;
        size_t getWritableBuffer(char** ppBuf) const;
        void clear();

    private:
        static std::deque<std::unique_ptr<char[] >> pool;

        char* pData;
        char* pBegin;
        char* pEnd;
    };

    size_t mySize;
    std::deque<Buffer> buffers;
};

#endif	/* SOCKETINSTREAM_HPP */

