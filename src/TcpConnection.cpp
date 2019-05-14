#include "TcpConnection.hpp"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "MessageHeader.hpp"
#include "MessageProcessor.hpp"

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
        
        CLUSTER_LOG(info) << "Connected to: " << boost::lexical_cast<std::string>(m_socket.remote_endpoint());

        boost::asio::async_read_until(m_socket, m_rxBuffer, END_OF_MESSAGE,
                                boost::bind(&TcpConnection::onRead, shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

    void TcpConnection::onRead(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (!error)
        {
            assert(bytes_transferred < m_rxBuffer.size() && "Size of the info transferred is more than the buffer size");
            CLUSTER_LOG(info) << "Received message [bytes transferred:" << bytes_transferred << "]";

            if(!MessageProcessor::processReceivedMessage(m_rxBuffer))
            {
                //delete the buffer if the process failed
                CLUSTER_LOG(info) << "Message process task failed";
                m_rxBuffer.consume(m_rxBuffer.size());
            }
            
        }
        else if (error == boost::asio::error::eof)
        {
            //
        }
        else
        {
            CLUSTER_LOG(error) << error.message();
        }

        m_socket.close();
    }
}
