#ifndef FEATURES_HPP
#define FEATURES_HPP
#include "ClusterIncludes.hpp"
#include <vector>
#include <boost/asio.hpp>

namespace ClusterController
{
    enum e_actions
    {
        e_doNothing = 0,
        e_onPressSend = 1,
        e_onPressActivate = 2,
        e_onPressExecute = 3
    };
    struct Led
    {
        Led(char* pin):pinNumber(atoi(pin)){}
        int pinNumber;
    };

    struct Button
    {
        Button(char* pin, boost::asio::ip::address c, e_actions a) : pinNumber(atoi(pin)), conn(c), action(a){}
        int pinNumber;
        boost::asio::ip::address conn;
        e_actions action;

    };

    class Features
    {
        public:
            void insertLed(char* value);

            void insertButton(char* value, char* conn, char* act);
            //useful for debug
            void printFeatures();

        private:
            std::vector<Button> m_buttonList;
            std::vector<Led> m_ledList;

    };
}
#endif