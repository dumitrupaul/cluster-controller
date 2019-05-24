#include "TcpConnection.hpp"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "MessageHeader.hpp"
#include "MessageProcessor.hpp"

namespace ClusterController
{

    TcpConnection::TcpConnection(boost::asio::io_service &io_service, boost::asio::ssl::context& context) : m_socket(io_service, context)
    {
    }

    TcpConnection::td_tcpConnPointer TcpConnection::create(boost::asio::io_service &io_service, boost::asio::ssl::context& context)
    {
        return td_tcpConnPointer(new TcpConnection(io_service, context));
    }

    boost::asio::ssl::stream<tcp::socket> &TcpConnection::getSocket()
    {
        return m_socket;
    }

    void TcpConnection::startHandshake()
    {
        CLUSTER_LOG(info) << "Starting new handshake";
        auto self(shared_from_this());
        m_socket.async_handshake(boost::asio::ssl::stream_base::server, 
        [this, self](const boost::system::error_code& error)
        {
            if (!error)
            {
            startServerConnection();
            }
            else
            {
            CLUSTER_LOG(error) << "Handshake failed reason: " << error.message();
            }
        });

    }
    void TcpConnection::startServerConnection()
    {
        
        CLUSTER_LOG(info) << "Connected to: " << boost::lexical_cast<std::string>(m_socket.lowest_layer().remote_endpoint());

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
        auto self(shared_from_this());
        m_socket.async_shutdown([this, self](const boost::system::error_code& error){
            // if(error == boost::asio::error::eof)
            //     CLUSTER_LOG(info) << "EOF";
            // CLUSTER_LOG(info) << "Server connection shutdown";
            m_socket.lowest_layer().close();

            // https://stackoverflow.com/questions/50693708/boost-asio-ssl-not-able-to-receive-data-for-2nd-time-onwards-1st-time-ok
            SSL_clear(m_socket.native_handle());
        });
        
    }

}