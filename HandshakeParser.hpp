/* 
 * File:   HandshakeParser.hpp
 * Author: ikki
 *
 * Created on March 10, 2015, 10:02 PM
 */

#ifndef HANDSHAKEPARSER_HPP
#define	HANDSHAKEPARSER_HPP

class HandshakeParser
{
public:
    explicit HandshakeParser();

    HandshakeParser(const HandshakeParser& other) = delete;
    HandshakeParser& operator=(const HandshakeParser& other) = delete;

    HandshakeParser(HandshakeParser&& other);
    HandshakeParser& operator=(HandshakeParser&& other);

    ~HandshakeParser();
};

#endif	/* HANDSHAKEPARSER_HPP */

