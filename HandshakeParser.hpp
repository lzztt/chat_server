/* 
 * File:   HandshakeParser.hpp
 * Author: ikki
 *
 * Created on March 10, 2015, 10:02 PM
 */

#ifndef HANDSHAKEPARSER_HPP
#define	HANDSHAKEPARSER_HPP

#include <string>
#include <unordered_map>

#include "SocketInStream.hpp"

class HandshakeParser
{
public:
    // HTTP response status
    enum class Status : int
    {
        PARSING = 0,
        OK = 200,
        BAD_REQUEST = 400
    };
    
    explicit HandshakeParser();

    HandshakeParser(const HandshakeParser& other) = delete;
    HandshakeParser& operator=(const HandshakeParser& other) = delete;

    HandshakeParser(HandshakeParser&& other);
    HandshakeParser& operator=(HandshakeParser&& other);

    ~HandshakeParser();

    HandshakeParser::Status parse(SocketInStream& in);
    
private:
    enum class State
    {
        METHOD,
        URI,
        HTTP_VERSION,
        CRLF,
        HEADER_NAME,
        HEADER_VALUE,
        ALMOST_END,
        END
    };
    
    enum class Method
    {
        UNKNOWN,
        GET
    };
    
    State myState;
    Status myStatus;
    Method myMethod;
    std::string myUri;
    std::string myHttpVersion;
    std::string myCurrentHeaderName;
    std::string myCurrentHeaderValue;
    std::unordered_map<std::string, std::string> myHeaders;
};

#endif	/* HANDSHAKEPARSER_HPP */

