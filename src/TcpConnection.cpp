#include "TcpConnection.hpp"
#include <boost/bind.hpp>
#include <iostream>
//#include "boost/log/trivial.hpp"

#define MAX_MESSAGE_SIZE 1024

namespace ClusterController
{

    TcpConnection::TcpConnection(boost::asio::io_service &io_service) : m_socket(io_service)
    {
    }

    TcpConnection::td_tcpConnPointer TcpConnection::create(boost::asio::io_service &io_service)
    {
        return td_tcpConnPointer(new TcpConnection(io_service));
    }

    tcp::socket &TcpConnection::getSocket()
    {
        return m_socket;
    }

    void TcpConnection::startServerConnection()
    {
        m_rxBuffer.resize(MAX_MESSAGE_SIZE);
        m_socket.async_read_some(boost::asio::buffer(&m_rxBuffer[0], MAX_MESSAGE_SIZE),
                                boost::bind(&TcpConnection::onRead, shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

    void TcpConnection::onRead(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (!error)
        {
            m_rxBuffer.resize(bytes_transferred);
           // BOOST_LOG_TRIVIAL(info) << "Read from: " << m_rxBuffer;
        }
        else if (error == boost::asio::error::eof)
        {
            //
        }
        else
        {
            std::cout << error.message();
        }
        m_socket.close();
    }
}