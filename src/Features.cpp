#include "Features.hpp"
#include <iostream>

namespace ClusterController
{
    void Features::insertLed(char* value)
    {
        return m_ledList.push_back(Led(value));
    }

    void Features::insertButton(char* value, char* c, char* act)
    {
        e_actions action = e_doNothing;
        if(std::string(act).compare("onPressSend") == 0)
        {
            action = e_onPressSend;
        }
        else if(std::string(act).compare("onPressActivate") == 0)
        {
            action = e_onPressActivate;
        }
        else if(std::string(act).compare("onPressExecute") == 0)
        {
            action = e_onPressExecute;
        }
        return m_buttonList.push_back(Button(value, boost::asio::ip::address::from_string(std::string(c)), action));
    }

    void Features::printFeatures()
    {
        for(std::vector<Led>::iterator it = m_ledList.begin(); it != m_ledList.end(); ++it)
        {
            std::cout << "\t LED: pin no: " << it->pinNumber << std::endl;
        }

        for(std::vector<Button>::iterator it = m_buttonList.begin(); it != m_buttonList.end(); ++it)
        {
            std::cout << "\t Button: pin no: " << it->pinNumber << " connected to: " 
                      << it->conn.to_string() << " action: " << it->action << std::endl;
        }
    }

}