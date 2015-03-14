/* 
 * File:   EventLoop.hpp
 * Author: ikki
 *
 * Created on February 17, 2015, 10:27 PM
 */

#ifndef EVENTLOOP_HPP
#define	EVENTLOOP_HPP

#include <unordered_map>

#include "Event.hpp"

class EventLoop
{
public:
    // singleton pattern
    static EventLoop& getInstance();

    EventLoop(const EventLoop& other) = delete;
    EventLoop& operator=(const EventLoop& other) = delete;

    EventLoop(EventLoop&& other) = delete;
    EventLoop& operator=(EventLoop&& other) = delete;

    ~EventLoop();
    
    bool registerEvent(const Event& e) noexcept;
    bool unregisterEvent(const Event& e) noexcept;
    void run() noexcept;
    void stop();
    
private:
    // private constructor for singleton
    EventLoop();
    
    int fd;
    int currentEventFd;
    std::unordered_map<int, Event> events;
};

#endif	/* EVENTLOOP_HPP */

