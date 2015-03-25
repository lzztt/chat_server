/* 
 * File:   App.hpp
 * Author: ikki
 *
 * Created on March 22, 2015, 11:07 PM
 */

#ifndef APP_HPP
#define	APP_HPP

#include <string>

class App
{
public:
    explicit App();

    App(const App& other) = delete;
    App& operator=(const App& other) = delete;

    App(App&& other);
    App& operator=(App&& other);

    ~App();

    void onOpen(unsigned int clientID);
    void onClose(unsigned int clientID);
    void onMessage(unsigned int clientID, std::string&& msg);
};

#endif	/* APP_HPP */

