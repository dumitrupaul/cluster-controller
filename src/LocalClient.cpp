#include "LocalClient.hpp"
#include <boost/bind.hpp>
#include <iostream>

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
        std::cout << "IP ADRESS:";
        std::cin >> m_ipAddress;
        std::cout << "ENTER MESSAGE: ";
        std::cin >> m_txBuffer;

        tcp::endpoint endPoint(boost::asio::ip::address::from_string(m_ipAddress), m_serverPort);
        m_socket.async_connect(endPoint, boost::bind(&LocalClient::onConnect, this, boost::asio::placeholders::error));
    }

    void LocalClient::onConnect(const boost::system::error_code &error)
    {
        if (!error)
        {
            //connection succeeded
            handleMessages();
        }
        else
        {
            if(error == boost::asio::error::connection_aborted) std::cout << "Device disconnected: ";
            std::cout << error.message() << std::endl;
            m_socket.close();
            startConnection();
        }
    }

    void LocalClient::handleMessages()
    {
        boost::asio::async_write(m_socket, boost::asio::buffer(m_txBuffer),
                                boost::bind(&LocalClient::onWrite, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

    void LocalClient::onWrite(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (!error)
        {
            std::cout << "Message transmitted successfully" << std::endl;
        }
        else
        {
            std::cout << error.message() << std::endl;
        }

        //close the socket after every message sent
        m_socket.close();
        startConnection();
    }
}