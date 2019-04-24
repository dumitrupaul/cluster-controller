#include "LocalClient.hpp"
#include <boost/bind.hpp>
#include <iostream>
#include "DeviceManager.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Message.hpp"

namespace ClusterController
{

    LocalClient::LocalClient(boost::asio::io_service &io_service, int serverPort) : 
                            m_serverPort(serverPort), 
                            m_socket(io_service),
                            connectionTimer(io_service)
    {
        startConnection();
    }

    void LocalClient::connectionTimerExpired()
    {
        std::cout << "Coldn't connect in time. Make sure batman-adv is properly configured "
                     "and the device is connected to the mesh" << std::endl;
        
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
                                "and the device is connected to the mesh: " << std::endl;
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
            std::cout << "Message transmitted successfully" << std::endl;
            m_txBuffer.consume(bytes_transferred);
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

        //m_txBuffer.clear();

        vector<string> names(DeviceManager::getInstance()->getNames());
        string selectedName;
        cout << endl << "Select a device from the list:" << endl;

        for (vector<string>::const_iterator i = names.begin(); i != names.end(); ++i)
        {
            cout << *i << endl;
        }

        cin >> selectedName;
        m_ipAddress = DeviceManager::getInstance()->getIPfromName(selectedName);
        cout << "ENTER MESSAGE: ";
        //std::cin >> m_txBuffer;
        Message msg(e_MSG_PING);
        if(msg.mouldPacketPayload(m_txBuffer)){
            return;
        }

    }
}