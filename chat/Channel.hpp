/* 
 * File:   Channel.hpp
 * Author: ikki
 *
 * Created on April 3, 2015, 10:18 PM
 */

#ifndef CHANNEL_HPP
#define	CHANNEL_HPP

#include <memory>

class User;

namespace chat {

class Channel
{
public:
    explicit Channel();

    Channel(const Channel& other) = delete;
    Channel& operator=(const Channel& other) = delete;

    Channel(Channel&& other);
    Channel& operator=(Channel&& other);

    ~Channel();

private:
    std::vector<std::weak_ptr<User>> users;
};

} // namespace chat

#endif	/* CHANNEL_HPP */

