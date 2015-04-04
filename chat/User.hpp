/* 
 * File:   User.hpp
 * Author: ikki
 *
 * Created on April 3, 2015, 10:22 PM
 */

#ifndef USER_HPP
#define	USER_HPP

#include <memory>

class Channel;

namespace chat {

class User
{
public:
    explicit User();

    User(const User& other) = delete;
    User& operator=(const User& other) = delete;

    User(User&& other);
    User& operator=(User&& other);

    ~User();

private:
    int socket;
    std::vector<std::weak_ptr<Channel>> channels;
    
    unsigned int uid;
    std::string name;
};

} // namespace chat

#endif	/* USER_HPP */

