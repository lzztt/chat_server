/* 
 * File:   Event.cpp
 * Author: ikki
 * 
 * Created on February 17, 2015, 10:20 PM
 */

#include <sys/epoll.h>
#include <utility>

#include "Event.hpp"
#include "Log.hpp"

Event::Event( const int fd, const uint32_t events, handler_t handler ) :
fd( fd ),
events( events | EPOLLRDHUP | EPOLLET ),
handler( std::move( handler ) )
{
    LOG_DEBUG << "created";
}

Event::Event( Event&& other ) :
fd( other.fd ),
events( other.events ),
handler( std::move( other.handler ) )
{
    LOG_DEBUG << "moved from " << &other;
}

Event& Event::operator=(Event&& other)
{
    LOG_DEBUG << "moved from " << &other;
    if ( this != &other )
    {
        fd = other.fd;
        events = other.events;
        handler = std::move( other.handler );
    }

    return *this;
}

Event::~Event( )
{
    LOG_DEBUG << "destroyed";
}

int Event::getFd( ) const
{
    return fd;
}

bool Event::isIn( ) const
{
    return events & EPOLLIN;
}

bool Event::isOut( ) const
{
    return events & EPOLLOUT;
}

bool Event::isError( ) const
{
    return (events & EPOLLRDHUP) || (events & EPOLLERR) || (events & EPOLLHUP);
}
