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
#include <vector>
#include <utility>

class WebSocketServerApp;

class SocketOutStream
{
    friend WebSocketServerApp;

public:

    SocketOutStream() = default;

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
    
    void add(std::vector<unsigned char>&& binary)
    {
        buffers.push_back(Buffer(std::move(binary)));
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

    class Data
    {
    public:

        enum class Type : int
        {
            TEXT,
            BINARY,
            FILE
        };

        struct File
        {
            int fd;
            size_t size;
        };

        Data(std::string&& str) :
        type(Data::Type::TEXT),
        str(std::move(str))
        {
        }

        Data(std::vector<unsigned char>&& binary) :
        type(Data::Type::BINARY),
        binary(std::move(binary))
        {
        }

        Data(Data::File file) :
        type(Data::Type::FILE),
        file(file)
        {
        }

        Data(const Data& other) = delete;
        Data& operator=(const Data& other) = delete;
        Data(Data&& other) = default;
        Data& operator=(Data&& other) = default;
        ~Data() = default;
        
        void clear()
        {
            str.clear();
            binary.clear();
            file = {0};
        }

        Type type;
        std::string str;
        std::vector<unsigned char> binary;
        File file;
    };

    class Buffer
    {
    public:
        Buffer(int fd);
        Buffer(std::string&& str);
        Buffer(std::vector<unsigned char>&& binary);
        Buffer(Data data);
        Buffer(Data&& data);

        Buffer(const Buffer& other) = delete;
        Buffer& operator=(const Buffer& other) = delete;
        Buffer(Buffer&& other);
        Buffer& operator=(Buffer&& other);
        ~Buffer();

        Data data;
        off_t offset;
    };

    std::deque<Buffer> buffers;
};

#endif	/* SOCKETOUTSTREAM_HPP */

