#include "LocalClient.hpp"
#include <boost/bind.hpp>
#include <iostream>
#include "DeviceManager.hpp"
#include "MessageProcessor.hpp"
#include <wiringPi.h>

#define DEBOUNCE_DELAY 1000

namespace ClusterController
{

    LocalClient::LocalClient(boost::asio::io_service &io_service, int serverPort, bool aMode) : 
                            m_serverPort(serverPort), 
                            autoMode(aMode),
                            m_socket(io_service),
                            sentMessageTimeInMs(0)                            
    {
        if(autoMode){
            std::cout << "Autonomous GPIO Mode engaged. No input necessary.\n";
            DeviceManager::getInstance()->getMyFeatures().printFeatures();
        }
        startConnection();
    }

    void LocalClient::startConnection()
    {  
        if(autoMode)
            handleGPIO();
        else
            handleInput();

        tcp::endpoint endPoint(m_ipAddress, m_serverPort);
        m_socket.async_connect(endPoint, boost::bind(&LocalClient::onConnect, this, boost::asio::placeholders::error));
    }

    void LocalClient::onConnect(const boost::system::error_code &error)
    {
        if (!error)
        {
            //connection succeeded
            writeMessage();
        }
        else
        {
            if(error == boost::asio::error::connection_aborted) std::cout << "Device disconnected: ";
            if(error == boost::asio::error::host_unreachable)
                std::cout << "Couldn't connect in time. Make sure batman-adv is properly configured "
                                "and the device is connected to the mesh: ";
            std::cout << error.message() << std::endl;
            m_socket.close();
            startConnection();
        }
    }

    void LocalClient::writeMessage()
    {
        boost::asio::async_write(m_socket, m_txBuffer.data(),
                                boost::bind(&LocalClient::onWrite, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

    void LocalClient::onWrite(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (!error)
        {
            std::cout << "Message transmitted successfully [size:"<< bytes_transferred << "]" << std::endl;
        }
        else
        {
            std::cout << error.message() << std::endl;
        }

        //close the socket after every message sent
        m_socket.close();
        startConnection();
    }

    void LocalClient::handleInput()
    {
        using namespace std;

        vector<string> names(DeviceManager::getInstance()->getNames());
        uint32_t selectedIdx;
        uint32_t msgType;
        uint32_t printIdx = 0U;

        cout << endl << "Select a device from the list:" << endl;

        for (vector<string>::const_iterator i = names.begin(); i != names.end(); ++i, ++printIdx)
        {
            cout << "\t[" << printIdx <<" - " << *i << "]" << endl;
        }

        cin >> selectedIdx;

        //Input validation
        while(selectedIdx < 0 || selectedIdx > names.size() - 1)
        {
            cout << "Invalid selection\n";
            cin >> selectedIdx;
        }
        
        m_ipAddress = DeviceManager::getInstance()->getIPfromName(names[selectedIdx]);
        cout << endl << "Select the type of message you want to send: \n\t[0 - PING] \n\t[1 - LED]\n";
      
        cin >> msgType;

        //TODO: Input validation

        if(!MessageProcessor::processSentMessageType(m_txBuffer, msgType))
            handleInput();

    }
    
    void LocalClient::handleGPIO()
    {
        Features& features = DeviceManager::getInstance()->getMyFeatures();
        
        std::vector<Button>& buttons = features.getButtonList();
        
        for(uint32_t buttonIdx = 0; buttonIdx < buttons.size(); ++buttonIdx)
        {        
            pinMode(buttons[buttonIdx].pinNumber, INPUT);
            pullUpDnControl(buttons[buttonIdx].pinNumber, PUD_UP);
        }
        
        bool buttonPressed = false;
        uint32_t currentTimeInMs;
        
        while(!buttonPressed)
        {
            currentTimeInMs = millis();
            if(currentTimeInMs - sentMessageTimeInMs > DEBOUNCE_DELAY)
            {
                bool returnValue = scanButtons(buttons);
                if(returnValue)
                    buttonPressed = true;
            }
        }
        
    }
    
    bool LocalClient::scanButtons(std::vector<Button>& buttons)
    {
        for(uint32_t buttonIdx = 0; buttonIdx < buttons.size(); ++buttonIdx)
        { 
            if(digitalRead(buttons[buttonIdx].pinNumber) == LOW)
            {
                sentMessageTimeInMs = millis();
                
                if(!MessageProcessor::processSentMessagePtr(m_txBuffer, buttons[buttonIdx].sendMsg))
                    return false;
                
                m_ipAddress = buttons[buttonIdx].conn;
                return true;
            }
        }
        return false;
        
        //if(digitalRead(buttons[1].pinNumber) == LOW)
        //{
            //MessageProcessor::processSentMessagePtr(m_txBuffer, buttons[0].sendMsg);
            //sentMessageTimeInMs = millis();
            //m_ipAddress = buttons[0].conn;
            
            //return true;
        //}
    }
}
