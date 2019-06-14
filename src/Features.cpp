#include "Features.hpp"
#include <iostream>
#include "MessagePing.hpp"
#include "MessageLed.hpp"

namespace ClusterController
{
    void Features::insertLed(char* value)
    {
        return m_ledList.push_back(Led(value));
    }

    void Features::insertButton(char* value, char* c, char* act, const char* msgChar)
    {
        e_actions action = e_doNothing;
        std::shared_ptr<Message_I> msg = std::make_shared<MessagePing>();
        
        if(std::string(act).compare("onPressSend") == 0)
        {
            action = e_onPressSend;
            if(std::string(msgChar).compare("MSG_LED_TOGGLE") == 0)
                msg = std::make_shared<MessageLed>(e_Toggle_Led);
            else if(std::string(msgChar).compare("MSG_LED_BLINK") == 0)
                msg = std::make_shared<MessageLed>(e_Blink_Led);
                
        }
        else if(std::string(act).compare("onPressActivate") == 0)
        {
            action = e_onPressActivate;
        }
        else if(std::string(act).compare("onPressExecute") == 0)
        {
            action = e_onPressExecute;
        }
        
        Button b(value, boost::asio::ip::address::from_string(std::string(c)), action);
        b.sendMsg = msg;
        
        m_buttonList.push_back(b);
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
    
    std::vector<Led>& Features::getLedList()
    {
        return m_ledList;
    }
    
    std::vector<Button>& Features::getButtonList()
    {
        return m_buttonList;
    }
    
    int Features::findLed(int pin)
    {
        for(uint32_t it = 0 ; it < m_ledList.size() ; ++it)
        {
            if(m_ledList[it].pinNumber == pin) return it;
        }
        
        return -1;
    }

}
