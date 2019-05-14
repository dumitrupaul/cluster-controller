#include "LocalClient.hpp"
#include <boost/bind.hpp>
#include <iostream>
#include "DeviceManager.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "MessageProcessor.hpp"

namespace ClusterController
{

    LocalClient::LocalClient(boost::asio::io_service &io_service, int serverPort) : 
                            m_serverPort(serverPort), 
                            m_socket(io_service)
    {
        startConnection();
    }

    void LocalClient::startConnection()
    {  
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
        uint32_t printIdx = 0;
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
}