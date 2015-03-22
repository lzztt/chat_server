/* 
 * File:   DataFrameParser.hpp
 * Author: ikki
 *
 * Created on March 10, 2015, 10:02 PM
 */

#ifndef DATAFRAMEPARSER_HPP
#define	DATAFRAMEPARSER_HPP

#include <memory>
#include "SocketInStream.hpp"

class DataFrameParser
{
public:

    struct Header
    {
        unsigned int fin : 1;
        unsigned int rsv1 : 1;
        unsigned int rsv2 : 1;
        unsigned int rsv3 : 1;
        unsigned int opcode : 4;
        unsigned int mask : 1;
        unsigned int payload : 7;
    };

    enum class Opcode : int
    {
        CONTINUATION = 0,
        TEXT = 1,
        BINARY = 2,
        CLOSE = 8,
        PING = 9,
        PONG = 10
    };

    enum class Status : int
    {
        PARSING,
        SUCCESS,
        UNRELEASED_DATA,
        MISSING_MASK_BIT,
        BAD_PAYLOAD_LENGTH
    };

    explicit DataFrameParser();

    DataFrameParser(const DataFrameParser& other) = delete;
    DataFrameParser& operator=(const DataFrameParser& other) = delete;

    DataFrameParser(DataFrameParser&& other);
    DataFrameParser& operator=(DataFrameParser&& other);

    ~DataFrameParser();

    Status parse(SocketInStream& in);
    
    size_t getData(std::unique_ptr<char[]>& pData);

private:

    enum class State : int
    {
        HEADER,
        PAYLOAD,
        MASK,
        DATA,
        END
    };
    State myState;
    Header myHeader;
    size_t myPayloadLength;
    char myMask[4];
    std::unique_ptr<char[] > myData;
    size_t myDataLength;
};

#endif	/* DATAFRAMEPARSER_HPP */

